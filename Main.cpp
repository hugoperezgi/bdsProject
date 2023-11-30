//Use g++ as the freaking compiler not trash gcc
#include <iostream>
#include <list>
#include <cmath>
#include <thread>
#include <stdint.h>
#include <stdlib.h>

#include "sim.cpp"

void runSimulation(uint8_t id, uint32_t it){
    
    std::cout << "Initiating simulation "<< std::to_string(id) << "\n";

    srand(time(NULL)/(id+1)); 
    sim simulation(id);

    simulation.runSimulation(it);

    simulation.getResults();
    simulation.getCompactResults();
    simulation.getSimulationData();
    
    std::cout << "Ending simulation "<< std::to_string(id) << "\n";

}



int main(int argc, char const *argv[]){

    uint8_t threadCount=0;
    uint32_t it=0;

    if (argc == 2){ // threads iterations
        std::string input = argv[1];
        if(strtod(argv[1],NULL)>255){
            std::cout << "Too many threads. Please dont.\n";
            return -1;
        }
        threadCount = (uint8_t) strtod(argv[1],NULL);
    }else if( argc==3 ){
        std::string input = argv[1];
        if(strtod(argv[1],NULL)>255){
            std::cout << "Too many threads. Please dont.\n";
            return -1;
        }
        threadCount = (uint8_t) strtod(argv[1],NULL);
        it = (uint32_t) strtod(argv[2],NULL);
    }else if( argc==1 ){
        threadCount=1;
    }else{
        std::cout << "Too many arguments, I break (¬_¬)\n";
        return -1;
    }


    if(threadCount==0){return -1;}
    if(it==0){it=25000;}

    std::thread simThreads[threadCount];

    for (uint8_t i = 0; i < threadCount; i++){
        simThreads[i]= std::thread(runSimulation,i,it);
    }
    
    for (uint8_t i = 0; i < threadCount; i++){
        simThreads[i].join();
    }

    return 0;
}
