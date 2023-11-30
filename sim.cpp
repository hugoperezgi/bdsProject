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
        std::list<gloop> simGloops;
        std::list<std::list<gloop>> pGloop;
        std::list<std::list<gloop>>::iterator it_pGloop;
        std::list<gloop>::iterator it;
        
        std::list<simulationReport> simReports;
        std::list<simulationReport>::iterator reportIt;

        uint16_t gloopCount,dedGloopCount, ageGloopCount,starvedGloopCount;
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

        void getReport(uint16_t);
        void sortBySpecies();
        void getDetailedReport(uint16_t);

        float a_rChance,a_dChance,a_mChance,a_lifeSpan,a_movRange,a_visionRange,a_foodBonus,a_foodDef;
        float a_age,a_DedAge;
        float min_rChance,min_dChance,min_mChance,min_lifeSpan,min_age,min_movRange,min_visionRange,min_foodBonus,min_foodDef;
        float max_rChance,max_dChance,max_mChance,max_lifeSpan,max_age,max_movRange,max_visionRange,max_foodBonus,max_foodDef;
        void avrgData();
        void minmaxData();

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
    this->simGloops.emplace_front(g);
    simId=0;
    gloopCount=1;
    
    simTime=1;
}

sim::sim(uint8_t id){

    gloop g;
    g.setRandomPosition();
    this->simGloops.emplace_front(g);
    simId=id;
    gloopCount=1;
    
    simTime=1;
}

sim::~sim(){
}

void sim::addMoreGloops(gloop parent){

    gloop g(parent.child());
    this->simGloops.insert(it,g);
}

void sim::sortBySpecies(){
    for(this->it=simGloops.begin(); this->it!=this->simGloops.end();this->it++){
        for(this->it_pGloop = pGloop.begin(); this->it_pGloop!=pGloop.end(); this->it_pGloop++){
            std::list<gloop>::iterator it_Gloop = (*it_pGloop).begin();
            if((*it).getSpecies()==((*it_Gloop).getSpecies())){(*it_pGloop).emplace_back((*it));goto nextGloop;}
        }
        addNewSpecies((*it));    
        nextGloop:
        continue;
    }


}
 
#ifndef precGLOOP_THRESHOLD
    #define precGLOOP_THRESHOLD 20
#endif

void sim::gloopSpawner(gloop g){

    g.setRandomPosition();

    float r;
    this->gloopCount>precGLOOP_THRESHOLD ? r=1 : r = ((float)this->gloopCount/(2*precGLOOP_THRESHOLD));
     
    if(r==1 || ((rand()%101)*(1+r) >= 100)){return;}

    this->simGloops.emplace_back(g);
}

void sim::addNewSpecies(gloop g){
    std::list<gloop> sGloop;
    sGloop.emplace_front(g);
    this->pGloop.emplace_front(sGloop);
}

#ifndef REPORT_PERIOD
    #define REPORT_PERIOD 1000
#endif
#ifndef DETAILED_REPORT
    #define DETAILED_REPORT true
#endif
void sim::runSimulation(uint32_t maxTime){

    gloop newSpawn;
    newSpawn.setRandomPosition();
    
    this->a_DedAge=0;
    this->a_age=0;
    this->dedGloopCount=0;
    this->starvedGloopCount=0;
    this->ageGloopCount=0;
    uint8_t prc=0;

    do{

        this->generateFood();

        simGloops.sort(randomizeGloopOrder);

        for (this->it = simGloops.begin(); it!=simGloops.end(); this->it++){

            this->gloopDoYourThingy();

        }

        this->removeded();
        this->updateCount();


        gloopSpawner(newSpawn); 

        
        if(!DETAILED_REPORT && simTime%REPORT_PERIOD==0) this->getReport(simTime/REPORT_PERIOD);
        if(DETAILED_REPORT && simTime%100==0){
            this->a_age/=(float)this->ageGloopCount;
            this->a_DedAge/=(float)this->dedGloopCount;
            this->getDetailedReport(simTime/100);
            this->a_age=0;
            this->a_DedAge=0;
            this->dedGloopCount=0;    
            this->starvedGloopCount=0;
            this->ageGloopCount=0;       
        }

        uint8_t tmp= (uint8_t) ((float)simTime/(maxTime-1) *100);
        if(tmp!=prc || simTime==1) std::cout << std::to_string(tmp) << "%\n";
        prc=tmp;

        if(this->gloopCount>MAX_GLOOPS){ 
            std::cout << "Max gloops reached, gloop count :" << std::to_string(gloopCount) << "\n";
            break;
        }

        simTime++;

    }while(simTime!=maxTime);

}

void sim::gloopDoYourThingy(){

    (*it).yummi(); //consume food

    if((*it).checkDeathChance(gloopCount)){

        (*it).kill(); //al pozo
        
        if(DETAILED_REPORT && simTime%100==0){
            this->a_DedAge+=(float)(*it).age;
            (*it).getFoodLv()==0 ? this->starvedGloopCount++ : this->dedGloopCount++;
        }
        
    }else{

        this->goYum(); //look for food

        if((*it).checkReplicationChance(gloopCount)){ //Replication
            addMoreGloops((*it));
        }

        (*it).nonono();//Increase age of gloops
        if(DETAILED_REPORT && simTime%100==0){
            this->a_age+=(float)(*it).age;
            this->ageGloopCount++;
        }

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

            (rand()%10001 <= DEFAULT_FOODCHANCE*100) ? ( (rand()%101 <= DEFAULT_BONUSFOOD) ? food[i][j] = 3 : food[i][j] = 1 ) : food[i][j]=0;

        }
    } 
}

std::string sim::getResults(){

    removeded();
    updateCount();
    sortBySpecies();

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

void sim::getReport(uint16_t reportCount){ 

    removeded();
    updateCount();
    sortBySpecies();

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
    this->simGloops.remove_if(isGloopDed);
}

void sim::updateCount(){
    // this->gloopCount=0;
    this->gloopCount=this->simGloops.size();
    // for(this->it_pGloop = pGloop.begin(); this->it_pGloop!=pGloop.end(); this->it_pGloop++){
    //     if((*it_pGloop).empty()){continue;}
    //     std::list<gloop> ffs = (*it_pGloop);
    //     this->gloopCount+=ffs.size();
    // }
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
    uint16_t sz = this->pGloop.size();

        this->a_age=0;
        this->a_lifeSpan=0;
        this->a_rChance=0;
        this->a_dChance=0;
        this->a_mChance=0;
        this->a_visionRange=0;
        this->a_movRange=0;
        this->a_foodBonus=0;
        this->a_foodDef=0;

    for(this->it_pGloop = pGloop.begin(); this->it_pGloop!=pGloop.end(); this->it_pGloop++){
        if((*this->it_pGloop).empty()){continue;}
        std::list<gloop> thisMakesNoFuckingSenseAndIDKWhy = (*this->it_pGloop);
        this->it = thisMakesNoFuckingSenseAndIDKWhy.begin();
        gloop shiet = (*this->it);
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

void sim::minmaxData(){

    uint16_t sz = this->simGloops.size();

    for(this->it=this->simGloops.begin();this->it!=this->simGloops.end();this->it++){

        gloop shiet = (*this->it);

        if(this->it == this->simGloops.begin()){
            this->a_lifeSpan=0;
            this->a_rChance=0;
            this->a_dChance=0;
            this->a_mChance=0;
            this->a_visionRange=0;
            this->a_movRange=0;
            this->a_foodBonus=0;
            this->a_foodDef=0;
            this->min_lifeSpan=(float)shiet.lifeSpan;
            this->min_rChance=(float)shiet.replicationChance;
            this->min_dChance=(float)shiet.deathChance;
            this->min_mChance=(float)shiet.mutationChance;
            this->min_visionRange=(float)shiet.getRangeOfVision();
            this->min_movRange=(float)shiet.getSpeed();
            this->min_foodBonus=(float)shiet.getFoodBonus();
            this->min_foodDef=(float)shiet.getFoodDef();
            this->max_lifeSpan=(float)shiet.lifeSpan;
            this->max_rChance=(float)shiet.replicationChance;
            this->max_dChance=(float)shiet.deathChance;
            this->max_mChance=(float)shiet.mutationChance;
            this->max_visionRange=(float)shiet.getRangeOfVision();
            this->max_movRange=(float)shiet.getSpeed();
            this->max_foodBonus=(float)shiet.getFoodBonus();
            this->max_foodDef=(float)shiet.getFoodDef();
        }

        this->a_lifeSpan+=(float)shiet.lifeSpan/sz;
        if( this->max_lifeSpan<shiet.lifeSpan ) {this->max_lifeSpan=shiet.lifeSpan;}else if(this->min_lifeSpan>shiet.lifeSpan) {this->min_lifeSpan=shiet.lifeSpan;}
        this->a_rChance+=(float)shiet.replicationChance/sz;
        if(this->max_rChance<shiet.replicationChance)  {this->max_rChance=shiet.replicationChance;} else if(this->min_rChance>shiet.replicationChance) {this->min_rChance=shiet.replicationChance;}
        this->a_dChance+=(float)shiet.deathChance/sz;
        if (this->max_dChance<shiet.deathChance) { this->max_dChance=shiet.deathChance ;}else if(this->min_dChance>shiet.deathChance){ this->min_dChance=shiet.deathChance;}
        this->a_mChance+=(float)shiet.mutationChance/sz;
        if (this->max_mChance<shiet.mutationChance) { this->max_mChance=shiet.mutationChance ;}else if(this->min_mChance>shiet.mutationChance){ this->min_mChance=shiet.mutationChance;}
        this->a_visionRange+=(float)shiet.getRangeOfVision()/sz;
        if (this->max_visionRange<shiet.getRangeOfVision()) { this->max_visionRange=shiet.getRangeOfVision();} else if(this->min_visionRange>shiet.getRangeOfVision()) {this->min_visionRange=shiet.getRangeOfVision();}
        this->a_movRange+=(float)shiet.getSpeed()/sz;
        if (this->max_movRange<shiet.getSpeed()) { this->max_movRange=shiet.getSpeed();} else if(this->min_movRange>shiet.getSpeed()) {this->min_movRange=shiet.getSpeed();}
        this->a_foodBonus+=(float)shiet.getFoodBonus()/sz;
        if (this->max_foodBonus<shiet.getFoodBonus()) { this->max_foodBonus=shiet.getFoodBonus();} else if(this->min_foodBonus>shiet.getFoodBonus()) {this->min_foodBonus=shiet.getFoodBonus();}
        this->a_foodDef+=(float)shiet.getFoodDef()/sz;
        if (this->max_foodDef<shiet.getFoodDef()) { this->max_foodDef=shiet.getFoodDef();} else if(this->min_foodDef>shiet.getFoodDef()) {this->min_foodDef=shiet.getFoodDef();}

    }

}

void sim::getDetailedReport(uint16_t reportCount){ 

    removeded();
    updateCount();

    minmaxData();

    std::ofstream file;
    std::string fileName ="reports\\formatted_Simulation"+std::to_string((int)this->simId)+"_Data.txt";
    reportCount == 1 ? file.open(fileName) : file.open(fileName, std::fstream::out | std::fstream::app);

    if(reportCount==1) file << "Iterations,GloopCountTotal,Average_LifeSpan,Max_LifeSpan,Min_LifeSpan,Average_Replication%,Max_Replication%,Min_Replication%,Average_Death%,Max_Death%,Min_Death%,Average_Mutation%,Max_Mutation%,Min_Mutation%,Average_VisionRange,Max_VisionRange,Min_VisionRange,Average_MovementRange,Max_MovementRange,Min_MovementRange,Average_FoodBonus%,Max_FoodBonus%,Min_FoodBonus%,Average_FoodDeficit%,Max_FoodDeficit%,Min_FoodDeficit%,Average_Age,Average_DeathAge,Starved_gloops\n";
                            
    file <<std::to_string(reportCount)<<","<<this->gloopCount<<","
         <<this->a_lifeSpan<<","<<this->max_lifeSpan<<","<<this->min_lifeSpan<<","
         <<this->a_rChance<<","<<this->max_rChance<<","<<this->min_rChance<<","
         <<this->a_dChance<<","<<this->max_dChance<<","<<this->min_dChance<<","
         <<this->a_mChance<<","<<this->max_mChance<<","<<this->min_mChance<<","
         <<this->a_visionRange<<","<<this->max_visionRange<<","<<this->min_visionRange<<","
         <<this->a_movRange<<","<<this->max_movRange<<","<<this->min_movRange<<","
         <<this->a_foodBonus<<","<<this->max_foodBonus<<","<<this->min_foodBonus<<","
         <<this->a_foodDef<<","<<this->max_foodDef<<","<<this->min_foodDef<<","
         <<this->a_age<<","<<this->a_DedAge<<","<<this->starvedGloopCount<<"\n";
    
    file.close();
}
