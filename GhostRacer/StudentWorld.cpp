#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    bonus=5000;
    soulsSaved=0;
}


int StudentWorld::init()
{
    mGhostRacer = new class GhostRacer(this);
    Actor* p = nullptr; //used to initialize Actor vector
                                        //initializes yellow border lines
    double N = VIEW_HEIGHT/SPRITE_HEIGHT;
    for(int j=0;j<N;j++){
        p = new BorderLine(this,LEFT_EDGE, j*SPRITE_HEIGHT, IID_YELLOW_BORDER_LINE);       //left yellow lines
        Actors.push_back(p);
        p = new BorderLine(this,RIGHT_EDGE, j*SPRITE_HEIGHT, IID_YELLOW_BORDER_LINE);       //right yellow lines
        Actors.push_back(p);
    }
                               //initializes white border lines
    double M = VIEW_HEIGHT/(4*SPRITE_HEIGHT);
    for(int j2=0;j2<M;j2++){
        p = new BorderLine(this, LEFT_EDGE+ROAD_WIDTH/3, j2*4*SPRITE_HEIGHT, IID_WHITE_BORDER_LINE);           //left white lines
        Actors.push_back(p);
        p = new BorderLine(this, RIGHT_EDGE-ROAD_WIDTH/3, j2*4*SPRITE_HEIGHT, IID_WHITE_BORDER_LINE);           //right white lines
        Actors.push_back(p);
    }

    lasty = (M-1) * 4 * SPRITE_HEIGHT; //keeps track of when to add new border lines
    overlapHumanped=false; //keeps track of when to restart lvl due hit human ped
    soulsSaved=0;
    bonus=5000;
    return GWSTATUS_CONTINUE_GAME;
}



int StudentWorld::move()
{

    mGhostRacer->doSomething();                      //GhostRacer does something
    if(mGhostRacer->isDead()){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
        
    for(int i=0;i<Actors.size();i++){               //every Actor in Actors vector does something
        if(!Actors[i]->isDead())
            Actors[i]->doSomething();
        if(mGhostRacer->isDead()||overlapHumanped){
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if(getLevel()*2+5-soulsSaved==0){
            increaseScore(bonus);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    for(int i=0;i<Actors.size();i++){               //goes through vector, deletes every dead actor
        if(Actors[i]->isDead()){
            delete Actors[i];
            Actors.erase(Actors.begin()+i);
            i--;
        }
    }

    Actor* p = nullptr; //used to add new Actors
                                                    //adds new borderlines after they go off screen
    lasty = lasty - 4 - mGhostRacer->getVerticalSpeed(); //-4 is borderline const speed
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - lasty;
    if(delta_y >= SPRITE_HEIGHT){                                             //adds yellow border lines on left and right edge if last yellow line went off screen
        p = new BorderLine(this, LEFT_EDGE, new_border_y, IID_YELLOW_BORDER_LINE);
        Actors.push_back(p);
        p = new BorderLine(this, RIGHT_EDGE, new_border_y, IID_YELLOW_BORDER_LINE);
        Actors.push_back(p);
    }
    if(delta_y >= 4*SPRITE_HEIGHT){                                           //adds white border lines on the left and right lanes if last white border line went off screen
        p = new BorderLine(this, LEFT_EDGE+ROAD_WIDTH/3, new_border_y, IID_WHITE_BORDER_LINE);
        Actors.push_back(p);
        p = new BorderLine(this, RIGHT_EDGE-ROAD_WIDTH/3, new_border_y, IID_WHITE_BORDER_LINE);
        Actors.push_back(p);
        lasty = new_border_y;
    }
        
    
                            //adds new zombie cabs by chance
    addZombieCab();
                            //adds new oil slicks by chance
    int ChanceOilSlick = max(150-getLevel()*10,40);
    if(randInt(0,ChanceOilSlick-1)==0){
        p = new OilSlick(this, randInt(LEFT_EDGE,RIGHT_EDGE),VIEW_HEIGHT);
        Actors.push_back(p);
    }
    
                                                    //adds new zombiepeds by chance
    int ChanceZombiePed = max(100-getLevel()*10,20);
    if(randInt(0, ChanceZombiePed-1)==0){
        p = new ZombiePedestrian(this, randInt(0,VIEW_WIDTH), VIEW_HEIGHT);     //spawns zombie randomly on top of screen
        Actors.push_back(p);
    }
    //adds new humanpeds by chance
    int ChanceHumanPed = max(200-getLevel()*10,30);
    if(randInt(0, ChanceHumanPed-1)==0){
        p = new HumanPedestrian(this, randInt(0,VIEW_WIDTH), VIEW_HEIGHT);     //spawns human randomly on top of screen
        Actors.push_back(p);
    }
    
                            //adds new Holy Water Refill Godies by chance
    int ChanceOfHolyWater = 100+getLevel()*10;
    if(randInt(0,ChanceOfHolyWater-1)==0){
        p = new HolyWaterGoodie(this, randInt(LEFT_EDGE,RIGHT_EDGE), VIEW_HEIGHT);
        Actors.push_back(p);
    }
    
                        //adds new Lost Soul Goodies by chance
    int ChanceOfLostSoul = 100;
    if(randInt(0, ChanceOfLostSoul-1)==0){
        p = new SoulGoodie(this, randInt(LEFT_EDGE,RIGHT_EDGE), VIEW_HEIGHT);
        Actors.push_back(p);
    }
    
                                                                //updates game status at the top of the screen
    ostringstream oss;
    oss.setf(ios::fixed);
    oss.precision(0);
    oss<<"Score: "<<getScore()<<"  Lvl: "<<getLevel()<<"  Souls2Save: "<<getLevel()*2+5-soulsSaved<<"  Lives: "<<getLives()<<"  Health: "<<mGhostRacer->getHP()<<"  Sprays: "<<mGhostRacer->getNumSprays()<<"  Bonus: "<<bonus;
    string s = oss.str();
    setGameStatText(s);
    
    bonus--;    //decrements bonus every tick
    
    return GWSTATUS_CONTINUE_GAME;
}



void StudentWorld::cleanUp()
{
    for(vector<Actor*>::iterator it = Actors.begin(); it != Actors.end(); it++){ //deletes all dynamically alocated memory
        delete *it;
    }
    delete mGhostRacer;
    Actors.clear();//makes Actors empty vector
}

StudentWorld::~StudentWorld(){
    cleanUp();
}



int StudentWorld::collisionin96unitsofZombieCab(Actor* cab){ //retuns 0 if no actor, -1 if behind 1 if ahead
    for(int i=0;i<Actors.size();i++){
        if(Actors[i]->isCollisionAvoidanceWorthy()&&laneofActor(cab)!=-1 && laneofActor(cab)==laneofActor(Actors[i])){ //checks if in same lane and collision worthy
                if(cab->getVerticalSpeed()>mGhostRacer->getVerticalSpeed()&&Actors[i]->getY()>cab->getY()&&Actors[i]->getY()-cab->getY()<96){ //checks if within 96 pixels and ahead
                    cab->setVerticalSpeed(cab->getVerticalSpeed()-.5);//reduces cab speed
                    return 1;
                }
                else if(cab->getVerticalSpeed()<=mGhostRacer->getVerticalSpeed()&&Actors[i]->getY()<cab->getY()&&cab->getY()-Actors[i]->getY()<96){ //checks if within 96 pixels and behind
                    cab->setVerticalSpeed(cab->getVerticalSpeed()+.5);//increases cab speed
                    return -1;
                }
        }
    }
    return 0; //returns 0 when now actor ahead or behind zombie cab that is collision risk
}




void StudentWorld::addZombieCab(){//determine starting position and movement of zombie cab
    bool CabAdded = false; //keeps track of whether cab was added
    int lanesChecked = 0;
    int firstchecked,secondchecked;
    int ChanceVehicle = max(100-getLevel()*10,20);
    int cur_lane = randInt(1,3);
    firstchecked=secondchecked=cur_lane;
    if(randInt(0,ChanceVehicle-1)==0){
        while(!CabAdded&&lanesChecked!=3){
            Actor* temp = nullptr;
            for(int i=0;i<Actors.size();i++){//increments through actors, recording lowest y value of actor in same lane as cur_lane
                if(Actors[i]->isCollisionAvoidanceWorthy()&&cur_lane==laneofActor(Actors[i])){
                    if(temp==nullptr)
                        temp=Actors[i];
                    else if(temp->getY()>Actors[i]->getY())
                        temp=Actors[i];
                }
            }
            if(temp==nullptr)
                temp=mGhostRacer;
            if(cur_lane==laneofActor(mGhostRacer)&&mGhostRacer->getY()<temp->getY())  //checks if ghostracer is in lane of interest
                temp = mGhostRacer;
            if(temp==nullptr||temp->getY()>VIEW_HEIGHT/3){//if there was no actor in the lane or it is high enough, spawn in zombie cab
                Actor* p = new ZombieCab(this, LEFT_EDGE+ROAD_WIDTH/6+ROAD_WIDTH/3*(cur_lane-1),SPRITE_HEIGHT/2);
                p->setVerticalSpeed(mGhostRacer->getVerticalSpeed()+randInt(2,4));
                Actors.push_back(p);
                CabAdded = true;
                break;
            }
            temp = nullptr;
            for(int i=0;i<Actors.size();i++){//increments through actors, recording hightest y value of actor in same lane as cur_lane
                if(Actors[i]->isCollisionAvoidanceWorthy()&&cur_lane==laneofActor(Actors[i])){
                    if(temp==nullptr)
                        temp=Actors[i];
                    else if(temp->getY()<Actors[i]->getY())
                        temp=Actors[i];
                }
            }
            if(temp==nullptr)
                temp=mGhostRacer;
            if(cur_lane==laneofActor(mGhostRacer)&&mGhostRacer->getY()>temp->getY())  //checks if ghostracer is in lane of interest
                temp = mGhostRacer;
            if(temp==nullptr||temp->getY()<VIEW_HEIGHT*2/3){//if there was no actor in the lane or it is low enough, spawn in zombie cab
                Actor* p = new ZombieCab(this, LEFT_EDGE+ROAD_WIDTH/6+ROAD_WIDTH/3*(cur_lane-1),VIEW_HEIGHT-SPRITE_HEIGHT/2);
                p->setVerticalSpeed(mGhostRacer->getVerticalSpeed()-randInt(2,4));
                Actors.push_back(p);
                CabAdded=true;
                break;
            }
            lanesChecked++;
            if(lanesChecked==3)
                break;
            while(cur_lane==firstchecked||cur_lane==secondchecked){             //sets cur_lane to a new lane that has not been checked yet
                cur_lane = randInt(1,3);
            }
            secondchecked=cur_lane;
        }
    }
}


int StudentWorld::laneofActor(Actor* p){ //returns 1 if in lane 1, 2 if is lane 2, 3 if in lane 3, -1 if outside yellow lines
    if(p->getX()<LEFT_EDGE+ROAD_WIDTH/3 && p->getX()>=LEFT_EDGE)
        return 1;
    if(p->getX()>=LEFT_EDGE+ROAD_WIDTH/3 && p->getX()<RIGHT_EDGE-ROAD_WIDTH/3)
        return 2;
    if(p->getX()<RIGHT_EDGE && p->getX()>=RIGHT_EDGE-ROAD_WIDTH/3)
        return 3;
    return -1;
}




GhostRacer* StudentWorld::getOverlappingGhostRacer(Actor* a) const{
    double delta_x = abs(mGhostRacer->getX()-a->getX());                  //sees if ghost racer is "touching" sprite
    double delta_y = abs(mGhostRacer->getY()-a->getY());
    double sum_rad = mGhostRacer->getRadius()+a->getRadius();
    if(delta_x<sum_rad*.25 && delta_y<sum_rad*.6)
        return mGhostRacer;
    return nullptr;
}


bool StudentWorld::sprayOverlap(Actor* spray, Actor* a){
    double delta_x = abs(spray->getX()-a->getX());                  //sees if spray is "touching" sprite
    double delta_y = abs(spray->getY()-a->getY());
    double sum_rad = spray->getRadius()+a->getRadius();
    if(delta_x<sum_rad*.25 && delta_y<sum_rad*.6)
        return true;
    return false;
}


bool StudentWorld::sprayFirstAppropriateActor(Actor* a){
    for(int i=0;i<Actors.size();i++){
        if(Actors[i]->isSprayable() && sprayOverlap(a, Actors[i])){
            Actors[i]->beSprayedIfAppropriate();
            return true;
        }
    }
    return false;
}




void StudentWorld::addActor(Actor* a){
    Actors.push_back(a);
}
