#include <stdint.h>
#include <iostream>
#include <list>

class simulationReport{
    private:
        /* data */
    public:
        simulationReport(/* args */);
        ~simulationReport();

        uint8_t reportNO,simNO;
        uint32_t speciesCountTotal,speciesCountAlive;
        uint16_t totalGloopCount;
        
        // [gloopCountSpecies]::[speciesId::parentSpeciesId::lifeSpan::replicationChance::deathChance::mutationChance::0::MovementRange::rangeOfVision::foodBonus::hungerDeficit]
        std::list<std::string> speciesReport;
        std::list<std::string>::iterator it;

        std::string fullReport();

};

simulationReport::simulationReport(/* args */){
}

simulationReport::~simulationReport(){
}

std::string simulationReport::fullReport(){
    std::string s = "\nReport"+std::to_string((int)reportNO)+" of Sim"+std::to_string((int)simNO)+": Generated Species->"+std::to_string(speciesCountTotal)+" Species Alive->"+std::to_string(speciesCountAlive)+" GloopCount->"+std::to_string(totalGloopCount)+"\n";

    for(this->it=this->speciesReport.begin(); this->it!=this->speciesReport.end(); this->it++){
        std::string r = (*this->it);
        s+=r+"\n";
    }
    return s;
}
