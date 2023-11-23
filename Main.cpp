//Use g++ as the freaking compiler not trash gcc
#include <iostream>
#include <list>
#include <cmath>
#include <thread>
#include <stdint.h>
#include <stdlib.h>

#include "sim.cpp"

void runSimulation(uint8_t id){
    
    std::cout << "Initiating simulation "<< std::to_string(id) << "\n";

    srand(time(NULL)); 
    sim simulation(id);

    simulation.runSimulation(25000);

    simulation.getResults();
    simulation.getCompactResults();
    simulation.getSimulationData();
    
    std::cout << "Ending simulation "<< std::to_string(id) << "\n";

}



int main(int argc, char const *argv[]){


    uint8_t threadCount=0;

    if (argc == 2){
        std::string input = argv[1];
        if(strtod(argv[1],NULL)>255){
            std::cout << "Fuck no\n";
            return -1;
        }
        threadCount = (uint8_t) strtod(argv[1],NULL);
    }else if( argc==1 ){
        threadCount=10;
    }else{
        std::cout << "Too many arguments, I break :c\n";
        return -1;
    }

    if(threadCount==0){return -1;}

    std::thread simThreads[threadCount];

    for (uint8_t i = 0; i < threadCount; i++){
        simThreads[i]= std::thread(runSimulation,i);
    }
    
    for (uint8_t i = 0; i < threadCount; i++){
        simThreads[i].join();
    }

    return 0;
}
