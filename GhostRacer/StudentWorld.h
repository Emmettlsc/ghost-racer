
// A possible interface for StudentWorld.  You may use all, some, or none
// of this, as you wish.

// One of the design goals is to reduce the coupling between StudentWorld.h
// and Actor.h as much as possible.  Notice that in fact, StudentWorld.h
// does not need to include Actor.h at all, and knows only two names in the
// Actor hierarchy.

#ifndef STUDENTWORLD_INCLUDED
#define STUDENTWORLD_INCLUDED

#include "GameWorld.h"
#include <string>
const int LEFT_EDGE = ROAD_CENTER-ROAD_WIDTH/2;
const int RIGHT_EDGE = ROAD_CENTER+ROAD_WIDTH/2;
class Actor;
class GhostRacer;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();

      // Return a pointer to the world's GhostRacer.
    GhostRacer* getGhostRacer(){return mGhostRacer;}

      // Add an actor to the world.
    void addActor(Actor* a);

    bool sprayOverlap(Actor* spray, Actor* a); //returns true if 2 actors overlap
    
      // Record that a soul was saved.
    void recordSoulSaved(){soulsSaved++;}
    int getSoulsSaved() const {return soulsSaved;}

      // If actor a overlaps some live actor that is affected by a holy water
      // projectile, inflict a holy water spray on that actor and return true;
      // otherwise, return false.  (See Actor::beSprayedIfAppropriate.)
    bool sprayFirstAppropriateActor(Actor* a);

      // If actor a overlaps this world's GhostRacer, return a pointer to the
      // GhostRacer; otherwise, return nullptr
    GhostRacer* getOverlappingGhostRacer(Actor* a) const;
    
    int collisionin96unitsofZombieCab(Actor* cab);//returns 0 if no actor, -1 if behind, 1 if ahead 
    
    void setoverlapHumanped(){overlapHumanped=true;}
    
    void addZombieCab();
    
    int laneofActor(Actor* p);
    
private:
    GhostRacer* mGhostRacer;
    std::vector<Actor*> Actors;
    int bonus, soulsSaved;
    double lasty; //keeps track of when to add white and yellow border lines
    bool overlapHumanped;
};

#endif // STUDENTWORLD_INCLUDED
