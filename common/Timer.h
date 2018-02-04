//
//  Timer.hpp
//  marstcp
//
//  Created by meryn on 2017/09/07.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Timer_hpp
#define Timer_hpp

#include <condition_variable>
#include <atomic>
#include <chrono>
#include <memory>
#include <vector>
#include "common/Singleton.h"


namespace common {
    
    void timeAlarm(int signal);
    
    class Timer:public Singleton<Timer>{
    
    private:
        Timer(){}
        void init(){
            signal(SIGALRM, timeAlarm);
        }
        friend Singleton<Timer>;
        
    public:
        
        ~Timer();
        
        void SetTimeout(std::chrono::microseconds millis, bool stop = false);
        
        void Stop();
        
        int GetNewListenFd();
        
        void AlarmHandler();
        
        
        
    private:
        
        //Timer* m_timer;
        
        //int m_timerFd;
        
        //std::atomic<std::chrono::time_point<std::chrono::system_clock>> m_lastTimePt;
        
        std::vector<int> m_outerFds;
        std::vector<int> m_innerFds;
        
        std::mutex m_mtx;
        
    };
    
}



#endif /* Timer_hpp */
