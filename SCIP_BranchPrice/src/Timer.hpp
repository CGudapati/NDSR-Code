 //
//  Timer.hpp
//  GraphPeeling
//
//  Created by Naga V Gudapati on 05/12/2018.
//  Copyright © 2018 Naga V Gudapati. All rights reserved.


#ifndef Timer_hpp
#define Timer_hpp
#include <chrono>
#include <iostream>
#include <iomanip>

class Timer {
private:
    std::chrono::time_point<std::chrono::steady_clock> start , stop;
    
public:
    
    void startClock();
    void stopClock();
    void elapsedTime(std::string &message);
    void elapsedTime();
    
};
#endif /* Timer_hpp */


