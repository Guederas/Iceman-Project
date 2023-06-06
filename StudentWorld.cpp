#include "StudentWorld.h"
#include <random>
#include <algorithm>
#include <iomanip>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(std::string assetDir)
{
    return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp


StudentWorld::StudentWorld(std::string assetDir) : GameWorld(assetDir) {}

StudentWorld::~StudentWorld()
{
}

// GAME INITIALIZATION

int StudentWorld::init() {
    // initialize the ice field
    for (int x = 0; x <= 64; x++) {                              // width
        for (int y = 0; y < 60; y++) {                          // height
            if (x >= 30 && x <= 33 && y >= 4 && y <= 59) {
                m_iceField[y][x] = nullptr;                     // empty tunnel
            }
            else if(x >= 0 && x <= 64 && y >= 60 && y <= 64) {
                m_iceField[y][x] = nullptr;                     // leaving the top area as nullptr to avod crashes
            }
            else {
                m_iceField[y][x] = new Ice(this, x, y);
            }
        }
    }

    // B = num of Boulders, G = num of Gold Nuggets, L = num of Barrels of Oil
    // static cast used because getLevel() returns as unsigned int, and min and max functions would not work
    int B = std::min(static_cast<int>(getLevel()) + 2, 9);              // number of Boulders
    int G = std::max(5 - static_cast<int>(getLevel()) / 2, 2);          // number of Gold Nuggets
    int L = std::min(2 + static_cast<int>(getLevel()), 21);             // number of Barrels of oil
    
    // Adding Boulders
    for (int i = 0; i < B; i++) {
        int randX, randY;
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distributionX(0, 60);    // x = 0 to x = 60 inclusive
        std::uniform_int_distribution<int> distributionY(20, 56);   // y = 20 to y = 56 inclusive
        bool validCoordinates = false;
        do {
            randX = distributionX(generator);
            randY = distributionY(generator);
            // check if the generated coordinates are within the specified distance from any other object
            bool withinDistance = false;
            for (const auto& actor : m_actors) {
                if (sqrt(pow(actor->getX() - randX, 2) + pow(actor->getY() - randY, 2)) <= 6.0) {
                    withinDistance = true;
                    break;
                }
            }
            if (!withinDistance) {
                validCoordinates = true;
            }
            if (randX > 26 && randX < 34) {     // if it is on tunnel, not valid
                validCoordinates = false;
            }
        } while (!validCoordinates);
        
        // clear the ice around the boulder
        clearIce(randX, randY);
        Boulder* boulder = new Boulder(this, randX, randY);
        addActor(boulder);
    }
    
    // Adding Gold Nuggets
    for (int i = 0; i < G; i++) {
        int randX, randY;
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distributionX(0, 60);    // x = 0 to x = 60 inclusive
        std::uniform_int_distribution<int> distributionY(0, 56);    // y = 0 to y = 56 inclusive
        bool validCoordinates = false;
        do {
            randX = distributionX(generator);
            randY = distributionY(generator);
            // check if the generated coordinates are within the specified distance from any other object
            bool withinDistance = false;
            for (const auto& actor : m_actors) {
                if (sqrt(pow(actor->getX() - randX, 2) + pow(actor->getY() - randY, 2)) <= 6.0) {
                    withinDistance = true;
                    break;
                }
            }
            if (!withinDistance) {
                validCoordinates = true;
            }
            if (randX > 26 && randX < 34) {     // if it is on tunnel, not valid
                validCoordinates = false;
            }
        } while (!validCoordinates);
        
        GoldNugget* nugget = new GoldNugget(this, randX, randY, false);
        addActor(nugget);
    }
    
    // Adding Oil Barrels
    m_barrelsLeft = 0;
    for (int i = 0; i < L; i++) {
        int randX, randY;
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distributionX(0, 60);
        std::uniform_int_distribution<int> distributionY(0, 56);
        bool validCoordinates = false;
        do {
            randX = distributionX(generator);
            randY = distributionY(generator);
            // check if the generated coordinates are within the specified distance from any other object
            bool withinDistance = false;
            for (const auto& actor : m_actors) {
                if (sqrt(pow(actor->getX() - randX, 2) + pow(actor->getY() - randY, 2)) <= 6.0) {
                    withinDistance = true;
                    break;
                }
            }
            if (!withinDistance) {
                validCoordinates = true;
            }
            if (randX > 26 && randX < 34) {     // if it is on tunnel, not valid
                validCoordinates = false;
            }
        } while (!validCoordinates);
        
        OilBarrel* oilBarrel = new OilBarrel(this, randX, randY);
        addActor(oilBarrel);
        addBarrel();
    }
    
    // initialize IceMan
    m_iceman = new IceMan(this, 30, 60);
    
    return GWSTATUS_CONTINUE_GAME;
}


// GAME ACTIONS DURING EACH TICK

int StudentWorld::move() {
    
    // update the game status text
    updateDisplayText();
    
    // give each Actor a chance to do something
    m_iceman->doSomething();
    for (auto& actor : m_actors) {
        if (actor->isAlive()) {
            actor->doSomething();
        }
        if (!m_iceman->isAlive()) {
            return GWSTATUS_PLAYER_DIED;
        }
        if (getBarrelsLeft() <= 0) {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    // ** NEED TO CREATE THE RANDOM GOODIES AND PROTESTERS
    //
    
    // removeDeadObjects
    removeDeadActors();
    
    // return the proper result
    if (!m_iceman->isAlive()) {
        return GWSTATUS_PLAYER_DIED;
    }
    
    // if all barrels are colected
    if (getBarrelsLeft() <= 0) {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    // if player has not completed level and is alive
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    for (auto& actor : m_actors) {
        delete actor;
    }
    for (int x = 0; x < 65; x++) {
        for (int y = 0; y < 65; y++) {
            delete m_iceField[y][x];
        }
    }
    delete m_iceman;
}

void StudentWorld::addActor(Actor *a) {
    m_actors.push_back(a);
}

void StudentWorld::clearIce(int x, int y) {
    for (int i = x; i < x + 4; i++) {
        for (int j = y; j < y + 4; j++) {
            if (!((x + 4) > 65 || (y + 4) > 65)) {      // avoid deleting not initizlied ice objects
                delete m_iceField[j][i];
                m_iceField[j][i] = nullptr;
            }
        }
    }
}

bool StudentWorld::canActorMoveTo(Actor* a, int x, int y) const {
    // check if out of bounds
    if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT) {
        return false;
    }
    // check if there are boulders on the way
    for (const auto& actor : m_actors) {
        if (dynamic_cast<Boulder*>(actor)) {
            if (isNearIceMan(actor, 2)) {
                switch (a->getDirection()) {                    // keeps anymation flowing and does not move through boulder
                    case GraphObject::up:                       // by testing different directions iceman is moving to
                        a->moveTo(a->getX(), a->getY() - 1);
                        return false;
                    case GraphObject::down:
                        a->moveTo(a->getX(), a->getY() + 1);
                        return false;
                    case GraphObject::left:
                        a->moveTo(a->getX() + 1, a->getY());
                        return false;
                    case GraphObject::right:
                        a->moveTo(a->getX() - 1, a->getY());
                        return false;
                    default:
                        break;
                }
            }
        }
    }
    // check if the proposed position overlaps with ice
    for (int i = x; i < x + SPRITE_WIDTH; i++) {
        for (int j = y; j < y + SPRITE_HEIGHT; j++) {
            if (m_iceField[j][i] != nullptr) {
                if (!dynamic_cast<IceMan*>(a)) {            // only IceMan can move through Ice
                    return false;
                }
            }
        }
    }
    return true;
}

int StudentWorld::annoyAllNearbyActors(Actor *annoyer, int points, int radius) {
    int actorsAnnoyed = 0;
    
    // annoyer coordinates
    int annoyerX = annoyer->getX();
    int annoyerY = annoyer->getY();
    
    // loop through actors
    for (const auto& actor : m_actors) {
        // calculate the euclidian distance
        int distance = sqrt(pow(actor->getX() - annoyerX, 2) + pow(actor->getY() - annoyerY, 2));
        
        if (distance <= radius) {   // ** MAYBE CHECK IF ACTOR CAN BE ANNOYED HERE **
            // annoy actor and increase the count
            actor->annoy(points);
            actorsAnnoyed++;
        }
    }
    return actorsAnnoyed;
}

void StudentWorld::revealAllNearbyObjects(int x, int y, int radius) {
    for (const auto& actor : m_actors) {
        // calculate the euclidian distance
        int distance = sqrt(pow(actor->getX() - x, 2) + pow(actor->getY() - y, 2));
        
        if (distance <= radius) {   // ** MAYBE CHECK IF ACTOR CAN BECOME VISIBLE HERE **
            // set visible
            actor->setVisible(true);
        }
    }
}

Actor* StudentWorld::findNearbyIceMan(Actor* a, int radius) const {
    int distance = sqrt(pow(m_iceman->getX() - a->getX(), 2) + pow(m_iceman->getY() - a->getY(), 2));
    
    if (distance <= radius) {
        return m_iceman;        // return pointer to iceman
    }
    return nullptr;             // return nullptr
}

Actor* StudentWorld::findNearbyPickerUpper(Actor *a, int radius) const {
    for (auto actor : m_actors) {
        if (actor->canPickThingsUp() && actor != a) {
            int distance = sqrt(pow(actor->getX() - a->getX(), 2) + pow(actor->getY() - a->getY(), 2));
            
            if (distance <= radius) {
                return actor;           // return pointer to nearby picker upper
            }
        }
    }
    return nullptr;
}

void StudentWorld::annoyIceMan() {
    // COME BACK HERE
}

void StudentWorld::giveIceManSonar() {
    m_iceman->addSonar();
}

void StudentWorld::giveIceManWater() {
    m_iceman->addWater();
}

bool StudentWorld::facingTowardIceMan(Actor* a) const {
    int actorX = a->getX();
    int actorY = a->getY();
    int iceManX = m_iceman->getX();
    int iceManY = m_iceman->getY();

    // check if the IceMan is in the same row or column as the actor
    if (actorX == iceManX || actorY == iceManY)
    {
        // check if the actor is facing toward the IceMan
        if (actorX == iceManX && actorY < iceManY && a->getDirection() == GraphObject::up)
            return true;
        else if (actorX == iceManX && actorY > iceManY && a->getDirection() == GraphObject::down)
            return true;
        else if (actorX < iceManX && actorY == iceManY && a->getDirection() == GraphObject::right)
            return true;
        else if (actorX > iceManX && actorY == iceManY && a->getDirection() == GraphObject::left)
            return true;
    }
    return false;
}

GraphObject::Direction StudentWorld::lineOfSightToIceMan(Actor* a) const {
    int actorX = a->getX();
    int actorY = a->getY();
    int iceManX = m_iceman->getX();
    int iceManY = m_iceman->getY();

    // check if the IceMan is in the same row or column as the actor
    if (actorX == iceManX || actorY == iceManY)
    {
        // check if the actor has a clear line of sight to the IceMan
        if (actorX == iceManX)
        {
            if (actorY < iceManY)
                return GraphObject::up;
            else if (actorY > iceManY)
                return GraphObject::down;
        }
        else if (actorY == iceManY)
        {
            if (actorX < iceManX)
                return GraphObject::right;
            else if (actorX > iceManX)
                return GraphObject::left;
        }
    }
    return GraphObject::none;
}

bool StudentWorld::isNearIceMan(Actor* a, int radius) const {
    // calculate the Euclidian distance
    int distance = sqrt(pow(a->getX() - m_iceman->getX(), 2) + pow(a->getY() - m_iceman->getY(), 2));

    // check if the distance is within the given radius
    if (distance <= radius) {
        return true;
    }
    return false;
}

GraphObject::Direction StudentWorld::determineFirstMoveToExit(int x, int y) {
    // COME BACK HERE LATER
    return GraphObject::none;
}

GraphObject::Direction StudentWorld::determineFirstMoveToIceMan(int x, int y) {
    // COME BACK HERE LATER
    return GraphObject::none;
}

void StudentWorld::updateDisplayText() {
    // get info
    int level = getLevel();
    int lives = getLives();
    int hp = m_iceman->getHitPoints() * 10; // since IceMan has 10 health, we *10 to keep track of percentage
    int squirts = m_iceman->getWater();
    int gold = m_iceman->getGold();
    int barrelsLeft = getBarrelsLeft();
    int sonar = m_iceman->getSonar();
    int score = getScore();
    
    // format the info using sstream
    ostringstream oss;
    oss << "Lvl: " << std::setw(2) << std::setfill(' ') << level << " ";
    oss << "Lives: " << lives << " ";
    oss << "Hlth: " << std::setw(3) << std::setfill(' ') << hp << "% ";
    oss << "Wtr: " << std::setw(2) << std::setfill(' ') << squirts << " ";
    oss << "Gld: " << std::setw(2) << std::setfill(' ') << gold << " ";
    oss << "Oil Left: " << std::setw(2) << std::setfill(' ') << barrelsLeft << " ";
    oss << "Sonar: " << std::setw(2) << std::setfill(' ') << sonar << " ";
    oss << "Scr: " << std::setw(6) << std::setfill('0') << score;
    
    // update the display text
    setGameStatText(oss.str());
}

void StudentWorld::addBarrel() {
    m_barrelsLeft++;
}

void StudentWorld::decBarrel() {
    m_barrelsLeft--;
}

unsigned int StudentWorld::getBarrelsLeft() {
    return m_barrelsLeft;
}

void StudentWorld::removeDeadActors() {
    auto a = m_actors.begin();
    while (a != m_actors.end()) {
        if (!(*a)->isAlive()) {
            delete *a;
            a = m_actors.erase(a);
        }
        else {
            ++a;
        }
    }
}

bool StudentWorld::isIce(int x, int y) const {
    // check if the given coordinates are within the bounds of the field
    if (x >= 0 && x < VIEW_WIDTH && y >= 0 && y < VIEW_HEIGHT) {
        // Check if there is Ice at the given coordinates
        if (m_iceField[x][y] != nullptr) {
            return true; // Ice found at the given coordinates
        }
    }
    return false; // no Ice found at the given coordinates or coordinates out of bounds
}
