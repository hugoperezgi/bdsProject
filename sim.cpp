#include <iostream>
#include <cmath>
#include <stdint.h>
#include "gloop.cpp"
#include <fstream>
#include <list>
#include "simulationReport.cpp"

// bool ffs(gloop g){
//     gloop* f = &g;
//     return gloop::isGloopDed(f);
// }

bool isGloopDed (const gloop& g) { return (!g.alive); }
bool randomizeGloopOrder(const gloop& g, const gloop& o){return (g.gloopId <= o.gloopId);}
bool isLstEpty(const std::list<gloop> l){ return l.empty();}
bool arrangeSpecies(const std::list<gloop> l1, const std::list<gloop> l2){
    std::list<gloop>::const_iterator it1 = l1.begin();
    std::list<gloop>::const_iterator it2=l2.begin();
    return ((*it1).species < (*it2).species);
}
bool randomizeSpecies(const std::list<gloop> l1, const std::list<gloop> l2){
    return (rand()%2==0);
}
bool sortDistances(const glpPos& f1, const glpPos& f2){
    return (f1.distance < f2.distance);
}

class sim{
    private:
        std::list<std::list<gloop>> pGloop;
        std::list<std::list<gloop>>::iterator it_pGloop;
        std::list<gloop>::iterator it;
        
        std::list<simulationReport> simReports;
        std::list<simulationReport>::iterator reportIt;

        uint16_t gloopCount;
        uint32_t simTime;
        uint8_t simId;

        uint8_t food[PLAYGROUND_SIZE_X][PLAYGROUND_SIZE_Y]; 

        void addMoreGloops(gloop);
        void addNewSpecies(gloop);
        void gloopSpawner(gloop);

        void gloopDoYourThingy();
        void removeded();
        void updateCount();
        void generateFood();
        void goYum();

        void getReport(uint8_t);

        float a_rChance,a_dChance,a_mChance,a_lifeSpan,a_age,a_movRange,a_visionRange,a_foodBonus,a_foodDef;
        void avrgData();

    public:
        sim();
        sim(uint8_t);
        ~sim();

        std::string getResults();
        std::string getCompactResults();

        std::string getSimulationData();

        void runSimulation(uint32_t);
};

sim::sim(){
    
    gloop g;
    g.setRandomPosition();
    std::list<gloop> sGloop;
    sGloop.emplace_front(g);
    pGloop.emplace(pGloop.begin(), sGloop);
    simId=0;
    gloopCount=1;
    
    simTime=1;
}

sim::sim(uint8_t id){

    gloop g;
    g.setRandomPosition();
    std::list<gloop> sGloop;
    sGloop.emplace_front(g);
    pGloop.emplace(pGloop.begin(), sGloop);
    simId=id;
    gloopCount=1;
    
    simTime=1;
}

sim::~sim(){
}

void sim::addMoreGloops(gloop parent){

    gloop g(parent.child());

    for(std::list<std::list<gloop>>::iterator it_pGloop_checkSpecies = pGloop.begin(); it_pGloop_checkSpecies!=pGloop.end(); it_pGloop_checkSpecies++){
        std::list<gloop>::iterator it_Gloop = (*it_pGloop_checkSpecies).begin();
        if(g.getSpecies()==((*it_Gloop).getSpecies())){(*it_pGloop_checkSpecies).insert(it,g);return;}
    }

    addNewSpecies(g);
}
 
#ifndef precGLOOP_THRESHOLD
    #define precGLOOP_THRESHOLD 20
#endif

void sim::gloopSpawner(gloop g){

    g.setRandomPosition();

    float r;
    this->gloopCount>precGLOOP_THRESHOLD ? r=1 : r = ((float)this->gloopCount/(2*precGLOOP_THRESHOLD));
     
    if(r==1 || ((rand()%101)*(1+r) >= 100)){return;}

    for(this->it_pGloop = pGloop.begin(); this->it_pGloop!=pGloop.end(); this->it_pGloop++){
        this->it = (*this->it_pGloop).begin();
        if(g.getSpecies()==((*this->it).getSpecies())){(*it_pGloop).emplace_back(g);return;}
    }
    addNewSpecies(g);
}

void sim::addNewSpecies(gloop g){
    std::list<gloop> sGloop;
    sGloop.emplace_front(g);
    pGloop.insert(it_pGloop,sGloop);
}

#ifndef REPORT_PERIOD
    #define REPORT_PERIOD 1000
#endif

void sim::runSimulation(uint32_t maxTime){

    gloop newSpawn;
    newSpawn.setRandomPosition();

    do{

        this->generateFood();

        for(this->it_pGloop = pGloop.begin(); it_pGloop!=pGloop.end(); this->it_pGloop++){
            if((*it_pGloop).empty()){continue;}
            for (this->it = (*it_pGloop).begin(); it!=(*it_pGloop).end(); this->it++){

                this->gloopDoYourThingy();

            }
            (*it_pGloop).sort(randomizeGloopOrder);
        }

        this->removeded();
        this->updateCount();

        if(this->gloopCount>MAX_GLOOPS){ 
            std::cout << "Max gloops reached, gloop count :" << std::to_string(gloopCount) << "\n";
            break;}

        gloopSpawner(newSpawn);

        pGloop.sort(randomizeSpecies);
        
        float prc=(float)simTime/maxTime *100;
        std::cout << std::to_string((int)prc) << "%\n";

        simTime++;

        if(simTime%REPORT_PERIOD==0) this->getReport(simTime/REPORT_PERIOD);

    }while(simTime!=maxTime);

}

void sim::gloopDoYourThingy(){

    if((*it).checkDeathChance(gloopCount)){

        (*it).kill(); //al pozo
        
    }else{

        (*it).yummi(); //consume food

        this->goYum(); //look for food

        if((*it).checkReplicationChance()){ //Replication
            addMoreGloops((*it));
        }

        (*it).nonono();//Increase age of gloops


    }
}

#ifndef DEFAULT_FOODCHANCE
    #define DEFAULT_FOODCHANCE 5
#endif
#ifndef DEFAULT_BONUSFOOD
    #define DEFAULT_BONUSFOOD 20
#endif
void sim::generateFood(){
    for (uint8_t i = 0; i < PLAYGROUND_SIZE_X; i++){
        for (uint8_t j = 0; j < PLAYGROUND_SIZE_Y; j++){

            (rand()%101 <= DEFAULT_FOODCHANCE) ? ( (rand()%101 <= DEFAULT_BONUSFOOD) ? food[i][j] = 3 : food[i][j] = 1 ) : food[i][j]=0;

        }
    } 
}

std::string sim::getResults(){

    removeded();
    updateCount();

    std::ofstream file;
    std::string fileName ="reports\\Report Simulation"+std::to_string((int)this->simId)+".txt";
    file.open(fileName);
    file << "Simulation number "<< std::to_string((int)this->simId) <<"\n";
    file << "Total Species count: "<< std::to_string(gloop::speciesCount) << " Remaining species count: " << pGloop.size() << " Gloop count: "<< std::to_string((int)this->gloopCount) << "\n";
    pGloop.sort(arrangeSpecies);
    for(this->it_pGloop = pGloop.begin(); it_pGloop!=pGloop.end(); this->it_pGloop++){
        if((*it_pGloop).empty()){continue;}
        std::list<gloop> thisMakesNoFuckingSenseAndIDKWhy = (*it_pGloop);
        this->it = thisMakesNoFuckingSenseAndIDKWhy.begin();
        gloop shiet = (*this->it);
        file << "Count: " << thisMakesNoFuckingSenseAndIDKWhy.size() << " Species data: " << shiet.printSpeciesData();
        file << "\n";
    }

    file.close();
    return fileName;
}

std::string sim::getCompactResults(){
    std::ofstream file;
    std::string fileName ="reports\\cSimulation"+std::to_string((int)this->simId)+".txt";
    file.open(fileName);
    file << "[gloopCountSpecies]::[speciesId::parentSpeciesId::lifeSpan::replicationChance::deathChance::mutationChance::0::speed::rangeOfVision::foodBonus::hungerDeficit]\n";
    for(this->reportIt = simReports.begin(); this->reportIt!=simReports.end(); this->reportIt++){
        simulationReport shiet = (*this->reportIt);
        file << shiet.fullReport();
    }

    file.close();
    return fileName;
}

void sim::getReport(uint8_t reportCount){ 

    removeded();
    updateCount();

    simulationReport r;
    r.reportNO=reportCount;
    r.simNO=this->simId;
    r.speciesCountTotal=gloop::speciesCount;
    r.speciesCountAlive=pGloop.size();
    r.totalGloopCount=this->gloopCount;

    pGloop.sort(arrangeSpecies);
    for(this->it_pGloop = pGloop.begin(); it_pGloop!=pGloop.end(); this->it_pGloop++){
        if((*it_pGloop).empty()){continue;}
        std::list<gloop> thisMakesNoFuckingSenseAndIDKWhy = (*it_pGloop);
        this->it = thisMakesNoFuckingSenseAndIDKWhy.begin();
        gloop shiet = (*this->it);

        r.speciesReport.emplace_back("["+std::to_string(thisMakesNoFuckingSenseAndIDKWhy.size())+"]::"+shiet.printCompactSpeciesData());
    }

    simReports.emplace_back(r);
}

void sim::removeded(){
    pGloop.remove_if(isLstEpty);
    for(this->it_pGloop = pGloop.begin(); it_pGloop!=pGloop.end(); this->it_pGloop++){
        (*it_pGloop).remove_if(isGloopDed);
    }
}

void sim::updateCount(){
    this->gloopCount=0;

    for(this->it_pGloop = pGloop.begin(); this->it_pGloop!=pGloop.end(); this->it_pGloop++){
        if((*it_pGloop).empty()){continue;}
        std::list<gloop> ffs = (*it_pGloop);
        this->gloopCount+=ffs.size();
    }
}

void sim::goYum(){
    glpPos gPos = (*it).gloop_Coords;
    uint8_t r = (*it).getSpeed();
    uint8_t food=0;

    if(this->food[gPos.gX][gPos.gY]==1){
        this->food[gPos.gX][gPos.gY]=0;
        food=25;
    }else if (this->food[gPos.gX][gPos.gY]==3){
        this->food[gPos.gX][gPos.gY]=0;
        food=50;
    }
    
    uint8_t tXH=0, tXL=0, tYH=0, tYL=0, fov=(*it).getRangeOfVision();
    if(r<fov) fov=r;
    gPos.gX >= PLAYGROUND_SIZE_X-fov ? tXH=PLAYGROUND_SIZE_X : tXH=gPos.gX+fov;
    gPos.gX <= fov ? tXL=0 : tXL=gPos.gX-fov;
    gPos.gY >= PLAYGROUND_SIZE_Y-fov ? tYH=PLAYGROUND_SIZE_Y : tYH=gPos.gY+fov;
    gPos.gY <= fov ? tYL=0 : tYL=gPos.gY-fov;

    std::list<glpPos> rFood1;
    std::list<glpPos> rFood3;
    
    for (uint8_t i = tXL; i <= tXH; i++){
        for (uint8_t j = tYL; j <= tYH; j++){
            glpPos a(gPos);
            a.setFood(i,j);
            if(this->food[i][j]==3)  {
                rFood3.emplace_back(a);
            }else if (this->food[i][j]==1) {
                rFood1.emplace_back(a);
            } 
        }
    }


    if(rFood1.empty() && rFood3.empty()){
        do{

            switch(rand()%4){
                case 0:
                    gPos.gX == PLAYGROUND_SIZE_X ? gPos.gX-- : gPos.gX++;
                    break;
                case 1:
                    gPos.gX == 0 ? gPos.gX++ : gPos.gX--;
                    break;
                case 2:
                    gPos.gY == PLAYGROUND_SIZE_Y ? gPos.gY-- : gPos.gY++;
                    break;
                case 3:
                    gPos.gY == 0 ? gPos.gY++ : gPos.gY--;
                    break;
            }

            if(this->food[gPos.gX][gPos.gY]==3){

                if(food==75){
                    this->food[gPos.gX][gPos.gY] = 1;
                    food=100;
                }else{
                    this->food[gPos.gX][gPos.gY] = 0;
                    food+=50;
                }
                    
            }else if(this->food[gPos.gX][gPos.gY]==1){
                if(food==75){
                    this->food[gPos.gX][gPos.gY] = 0;
                    food=100;
                }else{
                    this->food[gPos.gX][gPos.gY] = 0;
                    food+=25;
                }
            }

            if(food>=100){
                goto end;
            }
            --r;
        }while(r>0);
        goto end;
    }
    
    
    if(rFood1.empty() && rFood3.empty()){
        do{

            switch(rand()%4){
                case 0:
                    gPos.gX == PLAYGROUND_SIZE_X ? gPos.gX-- : gPos.gX++;
                    break;
                case 1:
                    gPos.gX == 0 ? gPos.gX++ : gPos.gX--;
                    break;
                case 2:
                    gPos.gY == PLAYGROUND_SIZE_Y ? gPos.gY-- : gPos.gY++;
                    break;
                case 3:
                    gPos.gY == 0 ? gPos.gY++ : gPos.gY--;
                    break;
            }

            if(this->food[gPos.gX][gPos.gY]==3){

                if(food==75){
                    this->food[gPos.gX][gPos.gY] = 1;
                    food=100;
                }else{
                    this->food[gPos.gX][gPos.gY] = 0;
                    food+=50;
                }
                    
            }else if(this->food[gPos.gX][gPos.gY]==1){
                if(food==75){
                    this->food[gPos.gX][gPos.gY] = 0;
                    food=100;
                }else{
                    this->food[gPos.gX][gPos.gY] = 0;
                    food+=25;
                }
            }

            if(food>=100){
                goto end;
            }
            --r;
        }while(r>0);
        goto end;
    }
    
    if(!rFood3.empty()){

        rFood3.sort(sortDistances);    
        for(std::list<glpPos>::iterator i = rFood3.begin(); i != rFood3.end(); i++){

            if((*i).dX+(*i).dY <= r){
                r-=((*i).dX+(*i).dY);

                gPos.gX=(*i).fX;
                gPos.gY=(*i).fY;

                if(food==75){
                    this->food[(*i).gX][(*i).gY] = 1;
                    food=100;
                }else{
                    this->food[(*i).gX][(*i).gY] = 0;
                    food+=50;
                }

            }

            if(food>=100 || r==0){goto end;}
        }
        
    }
    if(!rFood1.empty()){
        
        rFood1.sort(sortDistances);    
        for(std::list<glpPos>::iterator i = rFood1.begin(); i != rFood1.end(); i++){

            if((*i).dX+(*i).dY <= r){
                r-=((*i).dX+(*i).dY);

                gPos.gX=(*i).fX;
                gPos.gY=(*i).fY;

                if(food==75){
                    this->food[(*i).gX][(*i).gY] = 0;
                    food=100;
                }else{
                    this->food[(*i).gX][(*i).gY] = 0;
                    food+=25;
                }

            }

            if(food>=100 || r==0){goto end;}
        }
        
    }

    do{

        switch(rand()%4){
            case 0:
                gPos.gX == PLAYGROUND_SIZE_X ? gPos.gX-- : gPos.gX++;
                break;
            case 1:
                gPos.gX == 0 ? gPos.gX++ : gPos.gX--;
                break;
            case 2:
                gPos.gY == PLAYGROUND_SIZE_Y ? gPos.gY-- : gPos.gY++;
                break;
            case 3:
                gPos.gY == 0 ? gPos.gY++ : gPos.gY--;
                break;
        }

        if(this->food[gPos.gX][gPos.gY]==3){

            if(food==75){
                this->food[gPos.gX][gPos.gY] = 1;
                food=100;
            }else{
                this->food[gPos.gX][gPos.gY] = 0;
                food+=50;
            }
                
        }else if(this->food[gPos.gX][gPos.gY]==1){
            if(food==75){
                this->food[gPos.gX][gPos.gY] = 0;
                food=100;
            }else{
                this->food[gPos.gX][gPos.gY] = 0;
                food+=25;
            }
        }

        if(food>=100){
            goto end;
        }
        --r;
    }while(r>0);

end:

    (*it).gloop_Coords=gPos;

    (*it).feed(food);
}


std::string sim::getSimulationData(){
    std::ofstream file;
    std::string fileName ="reports\\simulation"+std::to_string((int)this->simId)+"_Data.txt";
    file.open(fileName);

    file << "Simulation "+std::to_string((int)this->simId)+" Data\n\n";
    file << "     "<<"Iterations ran: "<<std::to_string(this->simTime)<<"\n";
    file << "     "<<"Species generated: "<<std::to_string(gloop::speciesCount)<<"\n";
    file << "     "<<"Species remaining: "<<std::to_string(pGloop.size())<<"\n";
    file << "     "<<"Simulation Area: ( "<<std::to_string(PLAYGROUND_SIZE_X)<<" x "<<std::to_string(PLAYGROUND_SIZE_Y)<<" )\n";
    file << "     "<<"Food spawn chance: "<<std::to_string(DEFAULT_FOODCHANCE)<<"%\n";
    file << "     "<<"Double Food spawn chance: "<<std::to_string(DEFAULT_BONUSFOOD)<<"%\n";
    file << "     "<<"Gloop count: "<<std::to_string((int)this->gloopCount)<<" (Max of "<<std::to_string(MAX_GLOOPS)+")"<<"\n";
    file << "\n";
    file << "Initial Gloop stats:\n\n";
    file << "     "<<"Replication chance: "<<std::to_string(DEFAULT_RCHANCE)<<"% (Min: "<<std::to_string(MIN_RCHANCE)<<"% -Max: "<<std::to_string(MAX_RCHANCE)<<"%)\n";
    file << "     "<<"Death chance: "<<std::to_string(DEFAULT_DCHANCE)<<"% (Min: "<<std::to_string(MIN_DCHANCE)<<"%)\n";
    file << "     "<<"Mutation chance: "<<std::to_string(DEFAULT_MCHANCE)<<"% (Min: "<<std::to_string(MIN_MCHANCE)<<"% -Max: "<<std::to_string(MAX_MCHANCE)<<"%)\n";
    file << "     "<<"Starting Food: "<<std::to_string(DEFAULT_FOOD)<<"\n";
    file << "     "<<"Life span: "<<std::to_string(DEFAULT_LIFESPAN)<<"\n";
    file << "     "<<"Daily Food consumption: "<<std::to_string(DEFAULT_FOODCONSUMPTION)<<"\n";
    file << "     "<<"Movement range: "<<std::to_string(DEFAULT_MOVRANGE)<<" (Min: "<<std::to_string(MIN_MOVRANGE)<<"% -Max: "<<std::to_string(MAX_MOVRANGE)<<"%)\n";
    file << "     "<<"Range of vision: "<<std::to_string(DEFAULT_RANGEOFVISION)<<" (Min: "<<std::to_string(MIN_RANGEOFVISION)<<"% -Max: "<<std::to_string(MAX_RANGEOFVISION)<<"%)\n";
    file << "     "<<"Food bonus: "<<std::to_string(DEFAULT_FOODBONUS)<<"% (Min: "<<std::to_string(MIN_FOODBONUS)<<"% -Max: "<<std::to_string(MAX_FOODBONUS)<<"%)\n";
    file << "     "<<"Hunger deficit: "<<std::to_string(DEFAULT_HUNGERDEF)<<"% (Min: "<<std::to_string(MIN_HUNGERDEF)<<"% -Max: "<<std::to_string(MAX_HUNGERDEF)<<"%)\n";
    file << "\n";
    file << "Average Gloop species stats at the end of the simulation:\n\n";
    this->avrgData();
    file << "     "<<"Replication chance: "<<std::to_string(this->a_rChance)<<"%\n";
    file << "     "<<"Death chance: "<<std::to_string(this->a_dChance)<<"%\n";
    file << "     "<<"Mutation chance: "<<std::to_string(this->a_mChance)<<"%\n";
    file << "     "<<"Starting Food: "<<std::to_string(DEFAULT_FOOD)<<"\n";
    file << "     "<<"Life span: "<<std::to_string(this->a_lifeSpan)<<"\n";
    file << "     "<<"Daily Food consumption: "<<std::to_string(DEFAULT_FOODCONSUMPTION)<<"\n";
    file << "     "<<"Movement range: "<<std::to_string(this->a_movRange)<<"\n";
    file << "     "<<"Range of vision: "<<std::to_string(this->a_visionRange)<<"\n";
    file << "     "<<"Food bonus: "<<std::to_string(this->a_foodBonus)<<"%\n";
    file << "     "<<"Hunger deficit: "<<std::to_string(this->a_foodDef)<<"%\n";
    
    file.close();
    return fileName;
}

void sim::avrgData(){
    uint16_t sz = pGloop.size();

    for(this->it_pGloop = pGloop.begin(); it_pGloop!=pGloop.end(); this->it_pGloop++){
        if((*it_pGloop).empty()){continue;}
        std::list<gloop> thisMakesNoFuckingSenseAndIDKWhy = (*it_pGloop);
        this->it = thisMakesNoFuckingSenseAndIDKWhy.begin();
        gloop shiet = (*this->it);
        this->a_age+=(float)shiet.age/sz;
        this->a_lifeSpan+=(float)shiet.lifeSpan/sz;
        this->a_rChance+=(float)shiet.replicationChance/sz;
        this->a_dChance+=(float)shiet.deathChance/sz;
        this->a_mChance+=(float)shiet.mutationChance/sz;
        this->a_visionRange+=(float)shiet.getRangeOfVision()/sz;
        this->a_movRange+=(float)shiet.getSpeed()/sz;
        this->a_foodBonus+=(float)shiet.getFoodBonus()/sz;
        this->a_foodDef+=(float)shiet.getFoodDef()/sz;
    }


}