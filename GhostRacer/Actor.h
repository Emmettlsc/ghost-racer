// Possible interfaces for actors.  You may use all, some, or none
// of this, as you wish.

// One of the design goals is to reduce the coupling between StudentWorld.h
// and Actor.h as much as possible.  Notice that in fact, Actor.h
// does not need to include StudentWorld.h at all.

#ifndef ACTOR_INCLUDED
#define ACTOR_INCLUDED

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int depth);
    virtual void doSomething() = 0;//what actor does each tick
    virtual bool isSprayable() const = 0;//true if sprayable, false otherwise
    bool isDead() const{return m_isdead;}// Is this actor dead?
      // Mark this actor as dead.
    void setDead(){m_isdead=true;}
      // Get this actor's world
    StudentWorld* world() const{return StudentWorldp;}
      // Get this actor's vertical speed.
    double getVerticalSpeed() const{return vertSpeed;}
      // Set this actor's vertical speed.
    void setVerticalSpeed(double speed){vertSpeed=speed;}
      // If this actor is affected by holy water projectiles, then inflict that
      // affect on it and return true; otherwise, return false.
    virtual bool beSprayedIfAppropriate() = 0;
      // Does this object affect zombie cab placement and speed?
    virtual bool isCollisionAvoidanceWorthy() const{return false;} //false unless otherwise declared
      // Adjust the x coordinate by dx to move to a position with a y coordinate
      // determined by this actor's vertical speed relative to GhostRacser's
      // vertical speed.  Return true if the new position is within the view;
      // otherwise, return false, with the actor dead.
     bool moveRelativeToGhostRacerVerticalSpeed(double dx);                                      
private:
    StudentWorld* StudentWorldp;
    int vertSpeed;
    bool m_isdead;
};

class BorderLine : public Actor
{
public:
    BorderLine(StudentWorld* sw, double x, double y, int imageID);
    virtual bool isSprayable() const {return false;}
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate() {return false;}
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int hp);
    virtual bool isCollisionAvoidanceWorthy() const{return true;}

      // Get hit points.
    int getHP() const{return m_hp;}

      // Increase hit points by hp.
    void getHP(int hp){
        if(m_hp+hp>=100)
            m_hp=100;
        else
            m_hp+=hp;}

      // Do what the spec says happens when hp units of damage is inflicted.
      // Return true if this agent dies as a result, otherwise false.
     bool takeDamageAndPossiblyDie(int hp);

    virtual bool isSprayable() const {return true;}

    
private:
    int m_hp;
};

class GhostRacer : public Agent
{
public:
    GhostRacer(StudentWorld* sw);
    virtual void doSomething();
      // How many holy water projectiles does the object have?
    int getNumSprays() const{return m_sprays;};

      // Increase the number of holy water projectiles the object has.
    void increaseSprays(int amt){m_sprays = m_sprays + amt;}
    virtual bool beSprayedIfAppropriate() {return false;}

      // Spin as a result of hitting an oil slick.
    void spin();
    

private:
    int m_sprays;
};

class Pedestrian : public Agent
{
public:
    Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size);
      // Get the pedestrian's horizontal speed
    int getHorizSpeed() const{return horiz_speed;}
      // Set the pedestrian's horizontal speed
    void setHorizSpeed(int s){horiz_speed=s;}
      // Move the pedestrian.  If the pedestrian doesn't go off screen and
      // should pick a new movement plan, pick a new plan.
    int getmovement_plan() const {return movement_plan;}
    void setmovement_plan(int val){movement_plan=val;}
    void newmovementplan();
private:
    int horiz_speed;
    int movement_plan;
};

class HumanPedestrian : public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
};

class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
private:
    int tickstillgrunt;
};

class ZombieCab : public Agent
{
public:
    ZombieCab(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
private:
    int planlength, horiz_speed;
    bool notyetdamageGhostRacer;
};

class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir);
    virtual bool isSprayable() const {return false;}
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate(){return false;}
private:
    int maxtravel;
};

class GhostRacerActivatedObject : public Actor
{
public:
    GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir);
    
    virtual bool beSprayedIfAppropriate(){return false;}

};

class OilSlick : public GhostRacerActivatedObject
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool isSprayable() const{return false;}
};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool isSprayable() const{return true;}
    virtual bool beSprayedIfAppropriate();
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool isSprayable() const {return true;}
    virtual bool beSprayedIfAppropriate();
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
    SoulGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual int getSound() const{return SOUND_GOT_SOUL;}
    virtual bool isSprayable() const{return false;}
};

#endif // ACTOR_INCLUDED
