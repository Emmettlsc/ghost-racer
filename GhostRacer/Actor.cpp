#include "Actor.h"
#include "StudentWorld.h"
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

bool isoffScreen(Actor* p);

//Actor IMPLEMENTAIONS======================================================================================================================
Actor::Actor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int depth):GraphObject(imageID, x, y, dir, size, depth){
    StudentWorldp=sw;
    m_isdead=false;
}
bool Actor::moveRelativeToGhostRacerVerticalSpeed(double dx){
    int vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
    int horiz_speed = dx;
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    if(getX()<0||getY()<0||getX()>VIEW_WIDTH||getY()>VIEW_HEIGHT){
        setDead();
        return true;
    }
   return false;
}




//Agent IMPLEMENTATIONS================================================================================================================================
Agent::Agent(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int hp):Actor(sw, imageID, x, y, dir, size, 0){
    m_hp=hp;
}

bool Agent::takeDamageAndPossiblyDie(int hp){
    m_hp = m_hp - hp;
    if(m_hp<=0){
        setDead();
        return true;
    }
    return false;
}


//GHOST RACER IMPLEMENTAIONS ==========================================================================================================================
GhostRacer::GhostRacer(StudentWorld* sw):Agent(sw, IID_GHOST_RACER, 128, 32, 90, 4, 100){
    m_sprays = 10;
    setVerticalSpeed(0);
}

void GhostRacer::doSomething(){
    if(getHP()<=0||isDead()){
        setDead();
        return;}
    if(getX()<=LEFT_EDGE&&getDirection()>90){               //GhostRacer hits left side
        getHP(-10);
        if(getHP()<=0){
            setDead();
            world()->playSound(SOUND_PLAYER_DIE);
            return;
        }
        setDirection(82);
        world()->playSound(SOUND_VEHICLE_CRASH);
    }
    else if(getX()>=RIGHT_EDGE&&getDirection()<90){         //GhostRacer hits right side
        getHP(-10);
        if(getHP()<=0){
            setDead();
            world()->playSound(SOUND_PLAYER_DIE);
            return;
        }
        setDirection(98);
        world()->playSound(SOUND_VEHICLE_CRASH);
    }
    int keypress;                                   //evalutes the user's input
    if(world()->getKey(keypress))                   //complete actoins if there is input
    switch(keypress){
        case(KEY_PRESS_LEFT):
            if(getDirection()<114)                  //changes direction of GhostRacer sprite
                setDirection(getDirection()+8);
            break;
        case(KEY_PRESS_RIGHT):
            if(getDirection()>66)
                setDirection(getDirection()-8);
            break;
        case(KEY_PRESS_UP):                         //changes vert speed of GhostRacer sprite
            if(getVerticalSpeed()<5)
                setVerticalSpeed(getVerticalSpeed()+1);
            break;
        case(KEY_PRESS_DOWN):
            if(getVerticalSpeed()>-1)
                setVerticalSpeed(getVerticalSpeed()-1);
            break;
        case(KEY_PRESS_SPACE):
            if(getNumSprays()>0){
                Actor* temp = new Spray(world(),getX()+SPRITE_HEIGHT*cos(getDirection()*0.0174533), getY()+SPRITE_HEIGHT*sin(getDirection()*0.0174533),getDirection());//add spray shooting same direction as GhostRacer is facing
                world()->addActor(temp);
                world()->playSound(SOUND_PLAYER_SPRAY);
                increaseSprays(-1);
            }
            break;
    }
                                                                    //move the GhostRacer on screen using movement algorithm
    double max_shift_per_tick=4;
    int direction = getDirection();
    double delta_x = cos(direction*0.0174533) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x+delta_x,cur_y);
}


void GhostRacer::spin(){
    int temp = getDirection();
    int add = randInt(5,20);
    if(randInt(0,1)==0){    //randomly chooses spin direction
        if(temp+add>120)//spin counter-clockwise
            setDirection(120);
        else
            setDirection(temp+add);
    }
    else {  //spin clockwise
        if(temp-add<60)
            setDirection(60);
        else
            setDirection(temp-add);
    }
}
//BORDER LINE IMPLEMENTAIONS ========================================================================================================================================
BorderLine::BorderLine(StudentWorld* sw, double x, double y, int imageID):Actor(sw, imageID, x, y, 0, 2, 2){
    setVerticalSpeed(-4);
    
}

void BorderLine::doSomething() {
    moveRelativeToGhostRacerVerticalSpeed(0);
}



//PEDESTRIAN IMPLEMENTATIONS======================================================================================================================================
Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size):Agent(sw, imageID, x, y, 0, size, 2){
    movement_plan=0;
    setVerticalSpeed(-4);
    setHorizSpeed(0);
}

void Pedestrian::newmovementplan(){
    int new_horiz_speed = randInt(-3, 3);
    while(new_horiz_speed==0)               //ensures horiz speed != 0
        new_horiz_speed = randInt(-3, 3);
    setHorizSpeed(new_horiz_speed);
    setmovement_plan(randInt(4,32));
    if(new_horiz_speed<0)
        setDirection(180);
    else
        setDirection(0);
}



//HUMAN PEDESTRIAN IMPLEMENTATIONS =================================================================================================================================
HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y):Pedestrian(sw, IID_HUMAN_PED, x, y, 2){}


void HumanPedestrian::doSomething(){
    if(isDead())
        return;
    if(world()->getOverlappingGhostRacer(this)!=nullptr){  
        world()->setoverlapHumanped(); //tells world to decLives and end level
        return;
    }
    if(moveRelativeToGhostRacerVerticalSpeed(getHorizSpeed()))
        return;
    setmovement_plan(getmovement_plan()-1);         //return when movement plan is >0
    if(getmovement_plan()>0)
        return;
    else
        newmovementplan();//pick new movement plan when last plan ran out
}


bool HumanPedestrian::beSprayedIfAppropriate(){
    setHorizSpeed(getHorizSpeed()*-1);          //flips human ped sprite and horizontal movement direction around
    if(getDirection()==0)
        setDirection(180);
    else
        setDirection(0);
    world()->playSound(SOUND_PED_HURT);
    return true;
}



//ZOMBIE PEDESTRIAN IMPLEMENTATIONS =================================================================================================================================
ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y):Pedestrian(sw, IID_ZOMBIE_PED, x, y, 3){
    tickstillgrunt=0;
}

void ZombiePedestrian::doSomething(){
    if(isDead())
        return;
    if(world()->getOverlappingGhostRacer(this)!=nullptr){                                    //sees if touchng GhostRacer sprite and does damage
        world()->getGhostRacer()->takeDamageAndPossiblyDie(5);
        takeDamageAndPossiblyDie(2);
        world()->playSound(SOUND_PED_DIE);
        world()->increaseScore(150);
        return;
    }
    if(30>=abs(world()->getGhostRacer()->getX()-getX())&&getY()>world()->getGhostRacer()->getY()){        //sees if zombie is close to GhostRacer and above it
        setDirection(270);
        if(world()->getGhostRacer()->getX()>getX()) //if to left of racer
            setHorizSpeed(1);
        else if(world()->getGhostRacer()->getX()==getX())//if infront of racer
            setHorizSpeed(0);
        else                //if to right of racer
            setHorizSpeed(-1);
        tickstillgrunt--;
        if(tickstillgrunt<=0){
            world()->playSound(SOUND_ZOMBIE_ATTACK);
            tickstillgrunt=20;
        }
    }
                                    //zombie ped movement algorithm
    if(moveRelativeToGhostRacerVerticalSpeed(getHorizSpeed()))
        return;
    if(getmovement_plan()>0){
        setmovement_plan(getmovement_plan()-1);
        return;
    }
    else  //pick new movement plan for zombie
        newmovementplan();
}


bool ZombiePedestrian::beSprayedIfAppropriate(){
    if(takeDamageAndPossiblyDie(1)){
        world()->playSound(SOUND_PED_DIE);
        world()->increaseScore(150);
        if(randInt(1,5)==1){
            Actor* p = new HealingGoodie(world(), getX(), getY());
            world()->addActor(p);
        }
    }
    else {
        world()->playSound(SOUND_PED_HURT);
    }
    return true;
}

//ZOMBIE CAB IMPLEMENTATIONS====================================================================================================================
ZombieCab::ZombieCab(StudentWorld* sw, double x, double y):Agent(sw, IID_ZOMBIE_CAB, x, y, 90, 4, 3){
    planlength = 0;
    notyetdamageGhostRacer = true;
    horiz_speed=0;
}


void ZombieCab::doSomething(){
    if(isDead())
        return;
    if (world()->getOverlappingGhostRacer(this)!=nullptr){
        if(notyetdamageGhostRacer){     //does damage and crashes if not already damaged by GhostRacer
            world()->playSound(SOUND_VEHICLE_CRASH);
            world()->getGhostRacer()->takeDamageAndPossiblyDie(20);
            notyetdamageGhostRacer=false;
            world()->increaseScore(200);
            if(randInt(1,5)==1){
                Actor* p = new OilSlick(world(),getX(),getY()); //adds new oil slick if killed by ghostracer overlap
                world()->addActor(p);
            }
            if(getX()<world()->getGhostRacer()->getX()){        //crash to left wall
                horiz_speed=-5;
                setDirection(120 + randInt(0, 19));
            }
            else {          //crash to right wall
                horiz_speed=5;
                setDirection(60 + randInt(0, 19));
            }
        }
    }
    if(moveRelativeToGhostRacerVerticalSpeed(horiz_speed))
        return;
    if(world()->collisionin96unitsofZombieCab(this)!=0){
        return;
    }
    planlength--;
    if(planlength>0)
        return;
                    //new movement plan for zombiecab
    planlength = randInt(4,32);
    setVerticalSpeed(getVerticalSpeed()+randInt(-2,2));
}

bool ZombieCab::beSprayedIfAppropriate(){
    if(takeDamageAndPossiblyDie(1)){
        world()->playSound(SOUND_VEHICLE_DIE);
        if(randInt(1,5)==1){
            Actor* p = new OilSlick(world(),getX(),getY());     //adds new oilslick if killed by spray
            world()->addActor(p);
        }
        world()->increaseScore(200);
    }
    else
        world()->playSound(SOUND_VEHICLE_HURT);
    return true; 
}


//GHOSTRACER ACTIVATED OBJECT IMPLEMETNATIONS==========================================================================================
GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir):Actor(sw, imageID, x, y, dir, size, 2){
    setVerticalSpeed(-4);
}




//OIL SLICK IMPLEMENTATIONS==============================================================================================================
OilSlick::OilSlick(StudentWorld* sw, double x, double y):GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, randInt(2,5), 0){}

void OilSlick::doSomething(){
                                    //movement algorithm for oil slick
    if(moveRelativeToGhostRacerVerticalSpeed(0))
        return;
    if(world()->getOverlappingGhostRacer(this)!=nullptr){
        world()->playSound(SOUND_OIL_SLICK);
        world()->getGhostRacer()->spin();
    }
}


//HEALINGGOODIE IMPLEMENTATIONS===========================================================================================================
HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y):GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 1, 0){}

void HealingGoodie::doSomething(){
                //Healing Goodie movement algorithm
    if(moveRelativeToGhostRacerVerticalSpeed(0))
        return;
    GhostRacer* temp = world()->getOverlappingGhostRacer(this);
    if(temp!=nullptr){
        temp->getHP(10);
        setDead();
        world()->playSound(SOUND_GOT_GOODIE);
        world()->increaseScore(250);
    }
}

bool HealingGoodie::beSprayedIfAppropriate(){
    setDead();
    return true;
}



//HOLY WATER GOODIE IMPLEMENTATIONS===============================================================================================

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y):GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 2, 90){}

void HolyWaterGoodie::doSomething(){
                    //HolyWaterGoodie movement algorithm
    if(moveRelativeToGhostRacerVerticalSpeed(0))
        return;
    GhostRacer* temp = world()->getOverlappingGhostRacer(this);
    if(temp!=nullptr){
        temp->increaseSprays(10);
        setDead();
        world()->playSound(SOUND_GOT_GOODIE);
        world()->increaseScore(50);
    }
}


bool HolyWaterGoodie::beSprayedIfAppropriate(){
    setDead();
    return true;
}


//SOUL GOODIE IMPLEMENTATIONS===================================================================================================

SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y):GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 4, 0){}


void SoulGoodie::doSomething(){
                    //soul goodie movement algorithm
    if(moveRelativeToGhostRacerVerticalSpeed(0))
        return;
    GhostRacer* temp = world()->getOverlappingGhostRacer(this);
    if(temp!=nullptr){
        world()->recordSoulSaved();
        setDead();
        world()->playSound(SOUND_GOT_SOUL);
        world()->increaseScore(100);
    }
    setDirection(getDirection()+10);
}


//SPRAY IMPLEMENTATIONS========================================================================================
Spray::Spray(StudentWorld* sw, double x, double y, int dir):Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1, 1){
    maxtravel = 160;
}

void Spray::doSomething(){
    if(isDead())
        return;
    if(world()->sprayFirstAppropriateActor(this)){
        setDead();
        return;
    }
        moveForward(SPRITE_HEIGHT); //moves the projectile forward Sprite_height pixels
        maxtravel-=SPRITE_HEIGHT;
    if(getX()<0||getY()<0||getX()>VIEW_WIDTH||getY()>VIEW_HEIGHT){
        setDead();
        return;
    }
    if(maxtravel<=0)
        setDead();
}


