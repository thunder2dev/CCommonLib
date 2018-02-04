//
//  TimeQueue.h
//  marstcp
//
//  Created by meryn on 2017/09/05.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef TimeQueue_h
#define TimeQueue_h

#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "Timer.h"
#include "TimerEvent.h"
#include "callbacks.h"
#include "Heap.h"

namespace common {
    
    
    class TimeQueue{
        
    public:
        TimeQueue();
        
        std::shared_ptr<TimerEvent> AddTimeOutEvent(std::chrono::milliseconds _offset, TimeoutCallback _callback);
        std::shared_ptr<TimerEvent> AddIntervalTimeEvent(std::chrono::milliseconds _offset, std::chrono::milliseconds _inteval, TimeoutCallback _callback);
        
        void AdjustTimer();
        int TimerQueueCallback(std::shared_ptr<net::Channel> chan);
        int GetFd(){ return m_fd; }
        
    private:
        
        int m_fd;
        
        Timer *m_timer;
        std::chrono::time_point<std::chrono::system_clock> m_lastTp;
        
        std::function<bool(const TimerEvent::Ptr&, const TimerEvent::Ptr&)> comp;
        Heap<TimerEvent::Ptr, std::vector<TimerEvent::Ptr>, decltype(comp)> m_queue;
        
        
    };

}



#endif /* TimeQueue_h */















