#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int startX, int startY, Direction startDir,
        bool visible, int imageID, double size, int depth);

      // Action to perform each tick.
    virtual void doSomething() = 0;

      // Is this actor alive?
    bool isAlive() const;
    
      // Mark this actor as dead.
    void setDead();

      // Annoy this actor.
    virtual bool annoy(unsigned int amt);
    
      // Get this actor's world
    StudentWorld* getWorld() const;
    
      // Can other actors pass through this actor?
    virtual bool canActorsPassThroughMe() const;
    
      // Can this actor dig through Ice?
    virtual bool canDigThroughIce() const;
    
      // Can this actor pick items up?
    virtual bool canPickThingsUp() const;
    
      // Does this actor hunt the IceMan?
    virtual bool huntsIceMan() const;
    
      // Can this actor need to be picked up to finish the level?
    virtual bool needsToBePickedUpToFinishLevel() const;

      // Move this actor to x,y if possible, and return true; otherwise,
      // return false without moving.
    bool moveToIfPossible(int x, int y);
    
private:
    bool m_alive;
    StudentWorld* m_world;
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* world, int startX, int startY, Direction startDir,
                int imageID, unsigned int hitPoints);

      // Pick up a gold nugget.
    virtual void addGold() = 0;
    
      // How many hit points does this actor have left?
    unsigned int getHitPoints() const;

    virtual bool annoy(unsigned int amount);
    virtual bool canPickThingsUp() const;
    
private:
    unsigned int m_hitPoints;
};

class IceMan : public Agent
{
public:
    IceMan(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool annoy(unsigned int amount);
    virtual void addGold();
    virtual bool canDigThroughIce() const;

      // Pick up a sonar kit.
    void addSonar();

      // Pick up water.
    void addWater();

      // Get amount of gold
    unsigned int getGold() const;

      // Get amount of sonar charges
    unsigned int getSonar() const;

      // Get amount of water
    unsigned int getWater() const;
    
private:
    unsigned int m_gold;
    unsigned int m_sonar;
    unsigned int m_water;
};

class Protester : public Agent
{
public:
    Protester(StudentWorld* world, int startX, int startY, int imageID,
                unsigned int hitPoints, unsigned int score);
    virtual void doSomething();
    virtual bool annoy(unsigned int amount);
    virtual void addGold();
    virtual bool huntsIceMan() const;

      // Set state to having given up protest
    void setMustLeaveOilField();

      // Set number of ticks until next move
    void setTicksToNextMove();
    
private:
    bool m_mustLeaveField;
    int m_numberOfTicks;
    unsigned int m_score;
};

class RegularProtester : public Protester
{
public:
    RegularProtester(StudentWorld* world, int startX, int startY, int imageID);
    virtual void doSomething();
    virtual void addGold();
};

class HardcoreProtester : public Protester
{
public:
    HardcoreProtester(StudentWorld* world, int startX, int startY, int imageID);
    virtual void doSomething();
    virtual void addGold();
};

class Ice : public Actor
{
public:
    Ice(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
};

class Boulder : public Actor
{
public:
    Boulder(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool canActorsPassThroughMe() const;
    void setState(unsigned int state);
    unsigned int getState() const;
    void addTickCount();
    unsigned int getTickCount() const;
    
private:
    unsigned int m_state;    // 1 = stable, 2 = waiting, 3 = falling
    unsigned int m_numTicks;
};

class Squirt : public Actor
{
public:
    Squirt(StudentWorld* world, int startX, int startY, Direction startDir);
    virtual void doSomething();
    
private:
    unsigned int m_travelDistance;
};

class ActivatingObject : public Actor
{
public:
    ActivatingObject(StudentWorld* world, int startX, int startY, int imageID,
        int soundToPlay, bool activateOnPlayer,
        bool activateOnProtester, bool initiallyActive, unsigned int scoreToIncrease);
    virtual void doSomething();

      // Set number of ticks until this object dies
    void setTicksToLive(unsigned int ticks);
    
private:
    int m_soundToPlay;
    unsigned int m_ticksToLive;
    bool m_activateOnPlayer;
    bool m_activateOnProtester;
    bool m_initiallyActive;
    unsigned int m_scoreToIncrease;
};

class OilBarrel : public ActivatingObject
{
public:
    OilBarrel(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool needsToBePickedUpToFinishLevel() const;
};

class GoldNugget : public ActivatingObject
{
public:
    GoldNugget(StudentWorld* world, int startX, int startY, bool temporary);
    virtual void doSomething();
    
private:
    bool m_temporary;
};

class SonarKit : public ActivatingObject
{
public:
    SonarKit(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
};

class WaterPool : public ActivatingObject
{
public:
    WaterPool(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
};

#endif // ACTOR_H_

