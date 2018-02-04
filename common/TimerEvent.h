//
//  TimerEvent.hpp
//  marstcp
//
//  Created by meryn on 2017/09/11.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef TimerEvent_hpp
#define TimerEvent_hpp

#include <iostream>
#include "Utils.h"
#include <memory>
#include <chrono>
#include <functional>
#include "common.h"
#include "callbacks.h"

namespace common {
    
class Timer;
class TimeQueue;
class Channel;

class TimerEvent{
    
public:
    typedef std::shared_ptr<TimerEvent> Ptr;
    
    TimerEvent(std::chrono::time_point<std::chrono::system_clock> _tp, std::chrono::milliseconds _inteval, TimeoutCallback _callback);
    TimerEvent(std::chrono::time_point<std::chrono::system_clock> _tp, TimeoutCallback _callback);
    
    int64_t GetId() const { return m_timerId; }
    
    std::chrono::milliseconds GetInterval() const{return m_interval; }
    void SetInterval(std::chrono::milliseconds _interval) { m_interval = _interval; }
    
    std::chrono::time_point<std::chrono::system_clock> GetTimePoint() const{ return m_timePoint; }
    
    std::chrono::milliseconds UpdateInterval(std::chrono::milliseconds _interval) { std::chrono::milliseconds&& tmp = std::move(m_interval);  m_interval = std::move(_interval); return tmp; }
    
    bool IsStopped(){ return m_isIntervalStop; }
    void Stop();
    
    
    friend TimeQueue;
    friend bool operator== ( const TimerEvent &n1, const TimerEvent &n2);
    friend std::ostream& operator<<(std::ostream &strm, const TimerEvent &timer);
    
    
private:
    int64_t m_timerId;
    bool m_isInterval;
    bool m_isIntervalStop;
    std::chrono::time_point<std::chrono::system_clock> m_timePoint;
    std::chrono::milliseconds m_interval;
    TimeoutCallback m_callback;
    
    static std::atomic<int64_t> timerIdGenerator;
    
};


bool operator== ( const TimerEvent &n1, const TimerEvent &n2);

std::ostream& operator<<(std::ostream &strm, TimerEvent const& timer);

}
    
#endif /* TimerEvent_hpp */
