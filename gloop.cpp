#include <iostream>
#include <cmath>
#include <stdint.h>
#include "glpPos.cpp"
#include "simParams.h"


class gloop{
    private:
        /* data */
        
        uint8_t energy, food; 

        //Aggresivity {0-100}
        //Speed {5-15}
        //Range of vision
        //Food Bonus
        //Hunger Deficit
        uint8_t trait[5];

        bool checkChance(uint8_t);
        bool mutate();

    public:
        uint8_t lifeSpan, age;
        float replicationChance, deathChance, mutationChance;

        //Gloop Position
        glpPos gloop_Coords;

        static uint32_t speciesCount;
        uint32_t gloopId;
        uint32_t species;
        uint32_t parentSpecies;

        bool alive;

        bool checkReplicationChance();
        bool checkDeathChance(long);

        uint32_t getSpecies();

        void yummi();
        void feed(uint8_t);

        uint8_t getAggresivity();
        uint8_t getSpeed();
        uint8_t getRangeOfVision();
        uint8_t getFoodLv();
        uint8_t getFoodBonus();
        uint8_t getFoodDef();

        void nonono();
        bool isGloopDed(gloop&);

        void kill();
        void setRandomPosition();
        gloop child();

        std::string printSpeciesData();
        std::string printCompactSpeciesData();

        gloop();
        gloop(const gloop&);
        ~gloop();
};

uint32_t gloop::speciesCount;

gloop::gloop(){
    this->alive=true;
    this->food=DEFAULT_FOOD;
    this->energy=100;
    this->lifeSpan=DEFAULT_LIFESPAN;
    this->replicationChance=DEFAULT_RCHANCE;
    this->deathChance=DEFAULT_DCHANCE;
    this->mutationChance=DEFAULT_MCHANCE;
    this->age=0;
    this->parentSpecies=0;

    this->trait[0]=0; //Agresivity -fuckno
    this->trait[1]=DEFAULT_MOVRANGE; //Speed 
    this->trait[2]=DEFAULT_RANGEOFVISION;  //Range of Vision 
    this->trait[3]=DEFAULT_FOODBONUS;  //Food Bonus   
    this->trait[4]=DEFAULT_HUNGERDEF;  //Hunger Deficit

    this->gloopId=rand();
    this->species=0;
    gloop::speciesCount=0;
}

gloop::gloop(const gloop& g){
    this->alive=true;
    this->food=DEFAULT_FOOD;
    this->energy=100;
    this->age=0;
    this->lifeSpan=g.lifeSpan;
    this->replicationChance=g.replicationChance;
    this->deathChance=g.deathChance;
    this->mutationChance=g.mutationChance;
    this->species=g.species;
    this->parentSpecies=g.parentSpecies;
    this->gloopId=rand();

    this->gloop_Coords=glpPos(g.gloop_Coords);
    
    for(uint8_t i=0; i<sizeof(trait); i++){
        this->trait[i]=g.trait[i];
    }
}

gloop gloop::child(){
    gloop g(*this);
    if(g.mutate()){g.parentSpecies=this->species;}
    return g;
}


bool gloop::isGloopDed(gloop& g){
    return ! g.alive;
}

gloop::~gloop(){
}


bool gloop::mutate(){
    
    if(!checkChance(this->mutationChance)) return false;

huh:
    switch (rand()%2){
        case 0:
            
            switch (rand()%8){
                case 0:
                    this->lifeSpan*=1+(0.015+0.005*(rand()%5)); 
                    break;
                case 1:
                    this->replicationChance>=MAX_RCHANCE? this->replicationChance*=1-(0.015+0.005*(rand()%5)) : this->replicationChance*=1+(0.015+0.005*(rand()%5)); 
                    break;
                case 2:
                    this->deathChance*=1+(0.015+0.005*(rand()%5));
                    break;
                case 3:
                    this->mutationChance>=MAX_MCHANCE? this->mutationChance*=1-(0.015+0.005*(rand()%5)) : this->mutationChance*=1+(0.015+0.005*(rand()%5)); 
                    break;
                // case 4:
                //     this->trait[0]*=1+(0.015+0.005*(rand()%5));
                //     break;
                case 4: //Speed 5-15
                    this->trait[1]<=(MAX_MOVRANGE-2) ? this->trait[1]+=1+(rand()%2) : this->trait[1]<=(MAX_MOVRANGE-1) ? this->trait[1]+=(rand()%2) : this->trait[1]=15;
                    break;
                case 5: //Range of Vision 0-15
                    this->trait[2]<=(MAX_RANGEOFVISION-2) ? this->trait[2]+=1+(rand()%2) : this->trait[2]<=(MAX_RANGEOFVISION-1) ? this->trait[1]+=(rand()%2) : this->trait[2]=15;
                    break;
                case 6: //FoodBonus 0-15
                    this->trait[3]<=(MAX_FOODBONUS-2) ? this->trait[3]+=1+(rand()%2) : this->trait[3]<=(MAX_FOODBONUS-1) ? this->trait[3]+=(rand()%2) : this->trait[3]=15;
                    break;
                case 7: //HungerDeficit 0-15
                    this->trait[4]<=(MAX_HUNGERDEF-2) ? this->trait[4]+=1+(rand()%2) : this->trait[4]<=(MAX_HUNGERDEF-1) ? this->trait[4]+=(rand()%2) : this->trait[4]=15;
                    break;
            }//TODO fuck my life

            if(rand()%2==0) goto huh;

            gloop::speciesCount++;
            this->species=speciesCount;
            return true;

            break;
        case 1:
            
            switch (rand()%8){
                case 0:
                    this->lifeSpan*=1-(0.015+0.005*(rand()%5)); 
                    break;
                case 1:
                    this->replicationChance<=MIN_RCHANCE? this->replicationChance*=1+(0.015+0.005*(rand()%5)) : this->replicationChance*=1-(0.015+0.005*(rand()%5));
                    break;
                case 2:
                    this->deathChance<=MIN_DCHANCE? this->deathChance*=1+(0.015+0.005*(rand()%5)) : this->deathChance*=1-(0.015+0.005*(rand()%5));
                    break;
                case 3:
                    this->mutationChance<=MIN_MCHANCE? this->mutationChance*=1+(0.015+0.005*(rand()%5)) : this->mutationChance*=1-(0.015+0.005*(rand()%5));
                    break;
                // case 4:
                //     this->trait[0]*=1-(0.015+0.005*(rand()%5));
                //     break;
                case 4: //Speed 5-15
                    this->trait[1]>=(MIN_MOVRANGE+2) ? this->trait[1]-=(1+rand()%2) : this->trait[1]>=(MIN_MOVRANGE+1) ? this->trait[1]-=(rand()%2) : this->trait[1]=5;
                    break;
                case 5: //Range of Vision 0-15
                    this->trait[2]>=(MIN_RANGEOFVISION+2) ? this->trait[2]-=(1+rand()%2) : this->trait[2]>=(MIN_RANGEOFVISION+1) ? this->trait[1]-=(rand()%2) : this->trait[2]=0;
                    break;
                case 6: //FoodBonus 0-15
                    this->trait[3]>=(MIN_FOODBONUS+2) ? this->trait[3]-=(1+rand()%2) : this->trait[3]>=(MIN_FOODBONUS+1) ? this->trait[3]-=(rand()%2) : this->trait[3]=0;
                    break;
                case 7: //HungerDeficit 0-15
                    this->trait[4]>=(MIN_HUNGERDEF+2) ? this->trait[4]-=(1+rand()%2) : this->trait[4]>=(MIN_HUNGERDEF+1) ? this->trait[4]-=(rand()%2) : this->trait[4]=0;
                    break;
            }
            
            if(rand()%2==0) goto huh;

            gloop::speciesCount++;
            this->species=speciesCount;
            return true;

            break;
        default:
            return false;
            break;
    }
}

//value ranging from 25-100
void gloop::feed(uint8_t qty){
    (qty>=100) || ((this->food + qty)>100) ? this->food=100 : this->food=food+qty;
}

uint8_t gloop::getAggresivity(){
    return trait[0];
}

uint8_t gloop::getFoodLv(){return food;}

uint8_t gloop::getSpeed(){
    return trait[1];
}

uint8_t gloop::getFoodBonus(){return trait[3];}
uint8_t gloop::getFoodDef(){return trait[4];}

uint8_t gloop::getRangeOfVision(){
    return trait[2];
}

uint32_t gloop::getSpecies(){
    return species;
}

//Generates a random num, if random<=threshold -> True
bool gloop::checkChance(uint8_t threshold){
    uint8_t a = rand()%101;
    return (a<=threshold);
}

bool gloop::checkDeathChance(long gloops){
    long oc;
    ENABLE_FIXED_ocCoef ? ( oc=gloops/DEFAULT_OCROWDCOEF_FIXED ) : oc=0;      
    if(age==0){return false;}                           
    if(oc>=100 || this->food==0){return true;}           
    float dc=this->deathChance * (1+ (((float)(100-this->deathChance)/(this->deathChance))*((float)age/lifeSpan)));
    if((dc>=100) || (dc+oc >= 100)){return true;}else{return this->checkChance(dc+oc);}
}

#ifndef FOOD_THRESHOLD
    #define FOOD_THRESHOLD 75
#endif

bool gloop::checkReplicationChance(){
    float fP=0; 
    this->food >= FOOD_THRESHOLD ? fP=(float)this->trait[3]/100 : fP=-(float)this->trait[4]/100;
    float rC = (this->replicationChance) * (1+fP);
    return this->checkChance((uint8_t)rC);
}

void gloop::kill(){
    this->alive=false;
}

std::string gloop::printSpeciesData(){
    std::string s="SpeciesType: "+ std::to_string((int)this->species) +" ParentSpecies: "+ std::to_string((int)this->parentSpecies) +" LifeSpan: "+ std::to_string((int)this->lifeSpan) +" ReplicationChance: "+ std::to_string(this->replicationChance) +" DeathChance: "+ std::to_string(this->deathChance) +" MutationChance: "+ std::to_string(this->mutationChance);
    for(uint8_t i=0; i<sizeof(trait); i++){
        s+= " Trait"+std::to_string((int)i)+": "+std::to_string((int)trait[i]);
    }
    return s;
}

std::string gloop::printCompactSpeciesData(){
    std::string s="["+ std::to_string((int)this->species) +"::"+ std::to_string((int)this->parentSpecies) +"::"+ std::to_string((int)this->lifeSpan) +"::"+ std::to_string(this->replicationChance) +"::"+ std::to_string(this->deathChance) +"::"+ std::to_string(this->mutationChance);
    for(uint8_t i=0; i<sizeof(trait); i++){
        s+= "::"+std::to_string((int)trait[i]);
    }
    s+="]";
    return s;
}


void gloop::setRandomPosition(){
    
    this->gloop_Coords=glpPos(rand()%(PLAYGROUND_SIZE_X+1),rand()%(PLAYGROUND_SIZE_Y+1));

}

void gloop::nonono(){
    this->age++;
}

void gloop::yummi(){this->food=-DEFAULT_FOODCONSUMPTION;}