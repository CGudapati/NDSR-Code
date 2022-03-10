//
//  Timer.cpp
//  GraphPeeling
//
//  Created by Naga V Gudapati on 05/12/2018.
//  Copyright © 2018 Naga V Gudapati. All rights reserved.
//

#include "Timer.hpp"

void Timer::startClock() {
    start = std::chrono::steady_clock::now();
}

void Timer::stopClock() {
    stop = std::chrono::steady_clock::now();
}


void Timer::elapsedTime(std::string &message) {
    auto diff = stop - start;
    std::cout << "Elapsed time for " <<message<< " " << std::setprecision(13) <<std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
}

void Timer::elapsedTime(){
    auto diff = stop - start;
    std::cout << "Elapsed time is " << std::setprecision(13) << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
}




