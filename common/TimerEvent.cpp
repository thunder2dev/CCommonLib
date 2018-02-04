//
//  TimerEvent.cpp
//  marstcp
//
//  Created by meryn on 2017/09/11.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "TimerEvent.h"
#include <thread>

std::atomic<int64_t> common::TimerEvent::timerIdGenerator;

common::TimerEvent::TimerEvent(std::chrono::time_point<std::chrono::system_clock> _tp, std::chrono::milliseconds _inteval, TimeoutCallback _callback)
:m_timerId(++timerIdGenerator),
m_isInterval(true),
m_isIntervalStop(false),
m_timePoint(_tp),
m_interval(_inteval),
m_callback(_callback)
{
}

common::TimerEvent::TimerEvent(std::chrono::time_point<std::chrono::system_clock> _tp, TimeoutCallback _callback)
:m_timerId(++timerIdGenerator),
m_isInterval(false),
m_isIntervalStop(false),
m_timePoint(_tp),
m_callback(_callback)
{
}


bool common::operator== ( const common::TimerEvent &n1, const common::TimerEvent &n2)
{
    return n1.GetId()  == n2.GetId() ;
}

std::ostream& common::operator<<(std::ostream &strm, common::TimerEvent const&timer) {
    return strm << "timer id:" <<timer.GetId() <<" timer point:" << common::timePoint2str(timer.GetTimePoint()) << " interval:"<<timer.GetInterval().count() <<" ms";
}

void common::TimerEvent::Stop(){
    
    m_isIntervalStop = true;
    
}






