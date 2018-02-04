//
//  TimeQueue.cpp
//  marstcp
//
//  Created by meryn on 2017/09/05.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "TimeQueue.h"
#include "log/Log.h"
#include <unistd.h>
#include <cassert>
#include "Channel.h"
//#include "sys/timerfd.h"


const auto kTimeOutEpsMs = std::chrono::microseconds(10);

common::TimeQueue::TimeQueue()
:comp([](const TimerEvent::Ptr& e1, const TimerEvent::Ptr& e2){
    return e1->m_timePoint > e2->m_timePoint;
}),
m_queue(comp),
m_timer(Timer::GetInstance())
{
    m_fd = m_timer->GetNewListenFd();
}

std::shared_ptr<common::TimerEvent> common::TimeQueue::AddTimeOutEvent(std::chrono::milliseconds _offset, TimeoutCallback _callback){
    
    std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::now();
    tp += _offset;
    
    auto te = std::make_shared<TimerEvent>(tp, _callback);
    
    m_queue.push(te);
    
    AdjustTimer();
    
    return te;
}

std::shared_ptr<common::TimerEvent> common::TimeQueue::AddIntervalTimeEvent(std::chrono::milliseconds _offset, std::chrono::milliseconds _inteval, TimeoutCallback _callback){
    
    assert(_inteval.count() > 0);
    
    std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::now();
    tp += _offset;
    
    auto te = std::make_shared<TimerEvent>(tp, _inteval, _callback);
    
    m_queue.push(te);
    
    AdjustTimer();
    
    return te;
}

void common::TimeQueue::AdjustTimer(){
    
    if(m_queue.size() <= 0){
        return;
    }
    
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    const TimerEvent::Ptr ev = m_queue.top();
    
    if(ev->m_timePoint != m_lastTp){
        m_lastTp = ev->m_timePoint;
        
        auto dur = ev->m_timePoint - now + kTimeOutEpsMs;
        
        m_timer->SetTimeout(dur);
        
    }
    
}


int common::TimeQueue::TimerQueueCallback(std::shared_ptr<net::Channel> chan){
    
    char timerPipeData[1];
    
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    
    if(m_queue.size() <= 0){
        //ErrorLog <<"time queue is empty while alarm happens";
        read(chan->GetFd(), timerPipeData, 1);
        return 0;
    }
    
    std::shared_ptr<TimerEvent> ev = m_queue.top();
    
    auto dur = ev->m_timePoint - now;
    
    if(dur.count() > 0){
        return 0;
    }else{
        read(chan->GetFd(), timerPipeData, 1);
    }
    
    while ( dur.count() <= 0 && m_queue.size() > 0) {
        m_queue.pop();
        
        if( ev->m_callback(ev) != 0 ){
            ErrorLog <<"timer call back failed:" << *ev;
        }
        
        if( ev->m_isInterval && !ev->IsStopped()){
            ev->m_timePoint = ev->m_interval + now;
            m_queue.push(ev);
        }
        
        if(m_queue.size() > 0){
            ev = m_queue.top();
            dur = ev->m_timePoint - now;
        }
    }
    
    AdjustTimer();
    
    return 0;
}






































