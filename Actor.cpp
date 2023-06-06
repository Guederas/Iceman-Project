#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// FUNCTION DEFINITIONS FOR THE ATOR CLASS
Actor::Actor(StudentWorld* world, int startX, int startY, Direction startDir,
             bool visible, int imageID, double size, int depth)
: GraphObject(imageID, startX, startY, startDir, size, depth), m_alive(true), m_world(world)
{
    setVisible(visible);
}

bool Actor::isAlive() const {
    return m_alive;
}

void Actor::setDead() {
    m_alive = false;
}

bool Actor::annoy(unsigned int amt) {                   // virtual function
    return true;
}

StudentWorld* Actor::getWorld() const {
    return m_world;
}

bool Actor::canActorsPassThroughMe() const {            // virtual function
    return true;
}

bool Actor::canDigThroughIce() const {                  // virtual function
    return false;
}

bool Actor::canPickThingsUp() const {                   // virtual function
    return false;
}

bool Actor::huntsIceMan() const {                       // virtual function
    return false;
}

bool Actor::needsToBePickedUpToFinishLevel() const {    // virtual function
    return false;
}
 
bool Actor::moveToIfPossible(int x, int y) {
    if (getWorld()->canActorMoveTo(this, x, y)) {
        moveTo(x, y);
        return true;
    }
    return false;
}


// FUNCTION DEFINITIONS FOR THE AGENT CLASS
Agent::Agent(StudentWorld* world, int startX, int startY, Direction startDir,
             int imageID, unsigned int hitPoints)
: Actor(world, startX, startY, startDir, true, imageID, 1.0, 0), m_hitPoints(hitPoints)
{
}

unsigned int Agent::getHitPoints() const {
    return m_hitPoints;
}

bool Agent::annoy(unsigned int amount) {
    m_hitPoints -= amount;
    if (getHitPoints() <= 0) {
        return true;            // Agent is completely annoyed
    }
    return false;
}

bool Agent::canPickThingsUp() const {
    return true;
}


// FUNCTION DEFINITIONS FOR THE ICEMAN CLASS
IceMan::IceMan(StudentWorld* world, int startX, int startY)
: Agent(world, startX, startY, right, IID_PLAYER, 10), m_gold(0), m_sonar(1), m_water(5)
{
    setVisible(true);
}

void IceMan::doSomething() {
    if (!isAlive()) {
        return;
    }
    /* SOLVE THIS ONE LATER (ICEMAN CANNOT WALKT THROUGHT TUNNEL, ICE NOT CLEARING, FREEZES*/
    
    if (getWorld()->canActorMoveTo(this, getX(), getY()) ) {
        getWorld()->clearIce(getX(), getY());
        //getWorld()->playSound(SOUND_DIG);
    }
    
    int ch;
    if (getWorld()->getKey(ch)) {
        switch (ch) {
            case KEY_PRESS_ESCAPE:
                setDead();
                break;
                
            case KEY_PRESS_SPACE:
                if (getWater() > 0) {
                    // more stuff
                    Squirt* s = new Squirt(getWorld(), getX(), getY(), getDirection());
                    getWorld()->addActor(s);
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    m_water--;
                }
                break;
                
            case KEY_PRESS_LEFT:
                // left arrow key press
                if (getDirection() != left) {
                    setDirection(left);
                }
                else {
                    if (getX() > 0) {       // won't let IceMan move one squared out of bounds
                        moveToIfPossible(getX() - 1, getY());
                    }
                }
                break;

            case KEY_PRESS_RIGHT:
                // right arrow key press
                if (getDirection() != right) {
                    setDirection(right);
                }
                else {
                    if (getX() < 61) {      // since IceMan's is 4x4, can't move past the x = 60
                        moveToIfPossible(getX() + 1, getY());
                    }
                }
                break;

            case KEY_PRESS_UP:
                // up arrow key press
                if (getDirection() != up) {
                    setDirection(up);
                }
                else {
                    if (getY() < 61) {
                        moveToIfPossible(getX(), getY() + 1);
                    }
                }
                break;

            case KEY_PRESS_DOWN:
                // down arrow key press
                if (getDirection() != down) {
                    setDirection(down);
                }
                else {
                    if (getY() > 0) {
                        moveToIfPossible(getX(), getY() - 1);
                    }
                }
                break;
                
            case 'Z':
            case 'z':
                if (getSonar() > 0) {
                    getWorld()->revealAllNearbyObjects(getX(), getY(), 12);
                    getWorld()->playSound(SOUND_SONAR);
                    m_sonar--;
                    break;
                }
                    
            case KEY_PRESS_TAB:
                // add stuff here for drop gold nugget
                break;
                    
            default:
                break;
        }
    }
}


bool IceMan::annoy(unsigned int amount) {
    if (Agent::annoy(amount)) {
        setDead();
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
        return true;    // return true if IceMan is completely annoyed
    }
    return false;       // still got HP left
}

void IceMan::addGold() {
    // Maybe there is more stuff to do here
    m_gold++;
}

bool IceMan::canDigThroughIce() const {
    return true;
}

void IceMan::addSonar() {
    m_sonar++;
}

void IceMan::addWater() {
    m_water += 5;
}

unsigned int IceMan::getGold() const {
    return m_gold;
}

unsigned int IceMan::getSonar() const {
    return m_sonar;
}

unsigned int IceMan::getWater() const {
    return m_water;
}


// FUNCTION DEFINITIONS FOR THE PROTESTER CLASS

Protester::Protester(StudentWorld* world, int startX, int startY, int imageID,
                     unsigned int hitPoints, unsigned int score)
: Agent(world, startX, startY, left, imageID, hitPoints),m_mustLeaveField(false), m_score(score)
{
    setVisible(true);
    m_numberOfTicks = fmax(0, 3 - getWorld()->getLevel() / 4);
}

void Protester::doSomething() {
    if (!isAlive()) {
        return;
    }
    
}

bool Protester::annoy(unsigned int amount) {
    // COME BACK HERE LATER
    return false;
}

void Protester::addGold() {
    // COME BACK HERE LATER
}
bool Protester::huntsIceMan() const {
    return false;
}

void Protester::setMustLeaveOilField() {
    m_mustLeaveField = true; // COME BACK HERE
}

void Protester::setTicksToNextMove() {
    // COME BACK HERE
}


// FUNCTION DEFINITIONS FOR THE REGULAR PROTESTER CLASS
RegularProtester::RegularProtester(StudentWorld* world, int startX, int startY, int imageID)
: Protester(world, startX, startY, IID_PROTESTER, 5, 100)
{
    setTicksToNextMove();
}

void RegularProtester::doSomething() {
    // COME BACK HERE LATER
}

void RegularProtester::addGold() {
    // COME BACK HERE LATER
}


// FUNCTION DEFINITIONS FOR THE HARDCORE PROTESTER CLASS
HardcoreProtester::HardcoreProtester(StudentWorld* world, int startX, int startY, int imageID)
: Protester(world, startX, startY, IID_HARD_CORE_PROTESTER, 20, 250)
{
    setTicksToNextMove();
}

void HardcoreProtester::doSomething() {
    // COME BACK LATER
}

void HardcoreProtester::addGold() {
    // COME BACK LATER
}


// FUNCTION DEFINITIONS FOR THE ICE CLASS
Ice::Ice(StudentWorld* world, int startX, int startY)
: Actor(world, startX, startY, right, true, IID_ICE, .25, 3)
{
}

void Ice::doSomething() { /* Ice does not do something */}


// FUNCTION DEFINITIONS FOR THE BOULDER CLASS
Boulder::Boulder(StudentWorld* world, int startX, int startY)
: Actor(world, startX, startY, down, true, IID_BOULDER, 1.0, 1)
{
    m_state = 1;
    m_numTicks = 0;
}

void Boulder::doSomething() {
    if (!isAlive()) {
        return;
    }
    
    if (getState() == 1) {
        // check if ice on the 4 squares below
        for (int x = getX(); x < getX() + 4; x++) {
            if (getWorld()->isIce(x, getY() - 1)) {
                return;                 // do nothing if there is ice
            }
        }
        // if reaches this point, it means there is no Ice within the 4 squares below
        setState(2);
        return;
    }
    if (getState() == 2) {
        // wait 30 ticks before switching to falling
        addTickCount();
        if (getTickCount() >= 30) {
            setState(3);
            return;
        }
    }
    if (getState() == 3) {
        // cause MASSIVE DESTRUCTION MUAHAHA
        for (int x = getX(); x < getX() + 4; x++) {
            if (getWorld()->isIce(x, getY())) {
                setDead();
                return;
            }
        }
        
        moveToIfPossible(getX(), getY() - 1);
    }
}

void Boulder::setState(unsigned int state) {
    m_state = state;
}

unsigned int Boulder::getState() const {
    return m_state;
}

void Boulder::addTickCount() {
    ++m_numTicks;
}

unsigned int Boulder::getTickCount() const {
    return m_numTicks;
}

bool Boulder::canActorsPassThroughMe() const {
    // COME BACK HERE LATER
    return false;
}


// FUNCTION DEFINITIONS FOR THE SQUIRT CLASS
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction startDir)
: Actor(world, startX, startY, startDir, true, IID_WATER_SPURT, 1.0, 1)
{
    m_travelDistance = 4;
}

void Squirt::doSomething() {
    if (!isAlive()) {
        return;
    }
    if (getWorld()->annoyAllNearbyActors(this, 2, 3)) {
        setDead();
        return;
    }
    if (m_travelDistance == 0) {
        setDead();
        return;
    }
    switch (getDirection()) {
        case up:
            if (!moveToIfPossible(getX(), getY() + 1)) {
                setDead();
                return;
            }
            moveTo(getX(), getY() + 1);
            break;
        case down:
            if (!moveToIfPossible(getX(), getY() - 1)) {
                setDead();
                return;
            }
            moveTo(getX(), getY() - 1);
            break;
        case left:
            if (!moveToIfPossible(getX() - 1, getY())) {
                setDead();
                return;
            }
            moveTo(getX() - 1, getY());
            break;
        case right:
            if (!moveToIfPossible(getX() + 1, getY())) {
                setDead();
                return;
            }
            moveTo(getX() + 1, getY());
            break;
        default:
            break;
    }
    m_travelDistance--;
}


// FUNCTION DEFINITIONS FOR THE ACTIVATING OBJECT CLASS
ActivatingObject::ActivatingObject(StudentWorld* world, int startX, int startY, int imageID,
                                   int soundToPlay, bool activateOnPlayer,
                                   bool activateOnProtester, bool initiallyActive, unsigned int scoreToIncrease)
: Actor(world, startX, startY, right, true, imageID, 1.0, 2), m_soundToPlay(soundToPlay),
        m_activateOnPlayer(activateOnPlayer), m_activateOnProtester(activateOnProtester),
        m_initiallyActive(initiallyActive) {}

void ActivatingObject::doSomething() {
    if (!isAlive()) {
        return;
    }
    if (!isVisible() && getWorld()->isNearIceMan(this, 4)) {
        setVisible(true);
        return;
    }
    if (m_activateOnPlayer && getWorld()->isNearIceMan(this, 3)) {
        setDead();
        getWorld()->increaseScore(m_scoreToIncrease);
        getWorld()->playSound(m_soundToPlay);
        return;
    }
    // add do protester logic here
    
    if (!m_initiallyActive && m_ticksToLive > 0) {
        m_ticksToLive--;
        if (m_ticksToLive == 0) {
            setDead();
        }
    }
}

void ActivatingObject::setTicksToLive(unsigned int ticks) {
    m_ticksToLive = ticks;
}


// FUNCTION DEFINITIONS FOR THE OIL BARREL CLASS
OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY)
: ActivatingObject(world, startX, startY, IID_BARREL, SOUND_FOUND_OIL, true, false, true, 1000)
{
    setVisible(false);
}

void OilBarrel::doSomething() {
    ActivatingObject::doSomething();
    if (isAlive() && getWorld()->isNearIceMan(this, 3)) {
        getWorld()->decBarrel();
        return;
    }
}

bool OilBarrel::needsToBePickedUpToFinishLevel() const {
    return true;
}


// FUNCTION DEFINITIONS FOR THE GOLD NUGGET CLASS
GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool temporary)
: ActivatingObject(world, startX, startY, IID_GOLD, SOUND_GOT_GOODIE, true, false, true, 10), m_temporary(temporary)
{
    setVisible(false);
}

void GoldNugget::doSomething() {
    ActivatingObject::doSomething();
    
    if (isAlive() && getWorld()->isNearIceMan(this, 3)) {
        //getWorld()->playSound(SOUND_GOT_GOODIE);
        //getWorld()->findNearbyIceMan(this, 2)->addGold();
    }
}


// FUNCTION DEFINITIONS FOR THE SONAR KIT CLASS
SonarKit::SonarKit(StudentWorld* world, int startX, int startY)
: ActivatingObject(world, startX, startY, IID_SONAR, SOUND_GOT_GOODIE, true, false, false, 75)
{
    setTicksToLive(fmax(100, 300 - 10 * getWorld()->getLevel()));
    setVisible(true);
}

void SonarKit::doSomething() {
    ActivatingObject::doSomething();

    if (isAlive() && getWorld()->isNearIceMan(this, 3.0)) {
        //getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->giveIceManSonar();
    }
}


// FUNCTION DEFINITIONS FOR THE WATER POOL CLASS
WaterPool::WaterPool(StudentWorld* world, int startX, int startY)
: ActivatingObject(world, startX, startY, IID_WATER_POOL, SOUND_GOT_GOODIE, true, false, false, 100)
{
    setTicksToLive(fmax(100, 300 - 10 * getWorld()->getLevel()));
    setVisible(true);
}

void WaterPool::doSomething() {
    ActivatingObject::doSomething();

    if (isAlive() && getWorld()->isNearIceMan(this, 3.0)) {
        //getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->giveIceManWater();
    }
}

