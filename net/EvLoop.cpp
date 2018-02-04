//
//  EvLoop.cpp
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include <cassert>
#include "EvLoop.h"
#include "KQueuePoll.h"
#include "log/Log.h"
#include "Channel.h"
#include "common/TimeQueue.h"
#include <unistd.h>
#include <fcntl.h>
#include "common/FileUtil.h"


const std::thread::id net::EvLoop::NULL_THREAD_ID;

net::EvLoop::EvLoop(int _loopIdx)
:m_loopIdx(_loopIdx),
m_isRunning(false),
m_timeQueue(std::make_shared<common::TimeQueue>()),
m_hasSetDelayCb(false)
{
    m_poll = std::make_shared<KQueuePoll>();
}

void net::EvLoop::setupTimer(){
    
    Channel::Ptr chan = std::make_shared<Channel>(m_timeQueue->GetFd(), shared_from_this());
    chan->SetReadCallback(std::bind(&common::TimeQueue::TimerQueueCallback, m_timeQueue, chan));
    chan->EnableRead();
    chan->UpdateEvent();
    
    AddChannel(chan);
    
}


void net::EvLoop::Run(){
    this->m_tid = std::this_thread::get_id();
    this->m_isRunning = true;
    
    setupDelayCallbackFds();
    setupTimer();
    
    if(m_hasSetDelayCb){
        m_hasSetDelayCb = false;
        this->handleDelayJobs();
    }
    
    Loop();
}

int net::EvLoop::handleDelayJobs(){
    
    bool hasDelayJobs = true;
    do{
        
        std::function<void()> job;
        
        hasDelayJobs = this->m_delayCallbacks.Dequeue(job);
        
        if(hasDelayJobs) job();
        
    }while (hasDelayJobs);
    
    return 0;
    
}



void net::EvLoop::Loop(){
    
    for(;;){
        
        this->m_poll->Poll(common::MAX_POLL_WAIT_TIME_MS);
        
        handleDelayJobs();
        
    }
}


int net::EvLoop::AddChannel(std::shared_ptr<Channel> chan){
    if(IsInLoopThread()){
        addChannelInLoop(chan);
    }else{
        this->AddInLoopJob(std::bind(&EvLoop::addChannelInLoop, shared_from_this(), chan));
    }
    
    return 0;
}

int net::EvLoop::UpdateChannel(std::shared_ptr<Channel> chan){
    if(IsInLoopThread()){
        updateChannelInLoop(chan);
    }else{
        this->AddInLoopJob(std::bind(&EvLoop::updateChannelInLoop, shared_from_this(), chan));
    }
    
    return 0;
}


int net::EvLoop::DelChannel(std::shared_ptr<Channel> chan){
    if(IsInLoopThread()){
        delChannelInLoop(chan);
    }else{
        this->AddInLoopJob(std::bind(&EvLoop::delChannelInLoop, shared_from_this(), chan));
    }
    
    return 0;
}

int net::EvLoop::addChannelInLoop(std::shared_ptr<Channel> chan){
    assert(IsInLoopThread());
    
    return this->m_poll->AddChannel(chan);
    
}

int net::EvLoop::updateChannelInLoop(std::shared_ptr<Channel> chan){
    assert(IsInLoopThread());
    
    return this->m_poll->UpdateEvent(chan);
    
}


int net::EvLoop::delChannelInLoop(std::shared_ptr<Channel> chan){
    assert(IsInLoopThread());
    
    this->m_poll->DelChannel(chan);
    
    close(chan->GetFd());
    
    return 0;
}
   

bool net::EvLoop::IsInLoopThread(){
    
    return std::this_thread::get_id() == this->m_tid && m_isRunning;
    
}


void net::EvLoop::AddInLoopJob(std::function<common::DelayCallback> delayCB){
    
    bool noDelay = false;
    if(m_hasSetDelayCb.compare_exchange_strong(noDelay, true)){
        write(m_delayCbWriteFd, "d", 1);
    }
    
    this->m_delayCallbacks.Enqueue(std::move(delayCB));
}


void net::EvLoop::SetTimeOut(int _milli, common::TimeoutCallback _cb){
    
    if(IsInLoopThread()){
        this->setTimeOutInLoop(_milli, _cb);
    }else{
        this->AddInLoopJob(std::bind(&net::EvLoop::setTimeOutInLoop, shared_from_this(), _milli, _cb));
    }
}

void net::EvLoop::SetInterval(int _offset, int _interval, common::TimeoutCallback _cb){
    
    if(IsInLoopThread()){
        this->setIntervalInLoop(_offset, _interval, _cb);
    }else{
        this->AddInLoopJob(std::bind(&net::EvLoop::setIntervalInLoop, shared_from_this(), _offset, _interval, _cb));
    }
}

void net::EvLoop::setTimeOutInLoop(int _milli, common::TimeoutCallback _cb){
    
    this->m_timeQueue->AddTimeOutEvent(std::chrono::milliseconds(_milli), _cb);
    
}

void net::EvLoop::setIntervalInLoop(int _offset, int _interval, common::TimeoutCallback _cb){
    
    this->m_timeQueue->AddIntervalTimeEvent(std::chrono::milliseconds(_offset), std::chrono::milliseconds(_interval), _cb);
    
}



void net::EvLoop::setupDelayCallbackFds(){
    
    int fds[2];
    if(pipe(fds) < 0){
        ErrorLog <<"pipe delay cbs fd failed, err:"<< strerror(errno);
        exit(EXIT_FAILURE);
    }
    
    m_delayCbReadFd = fds[0];
    m_delayCbWriteFd = fds[1];
    
    common::setClosExec(m_delayCbReadFd);
    common::setClosExec(m_delayCbWriteFd);
    
    common::setNonBlocking(m_delayCbWriteFd);
    common::setNonBlocking(m_delayCbReadFd);
    
    Channel::Ptr chan = std::make_shared<Channel>(m_delayCbReadFd, shared_from_this());
    chan->SetReadCallback(std::bind(&net::EvLoop::delayCallbackCb, shared_from_this(), chan));
    chan->EnableRead();
    chan->UpdateEvent();
    
    AddChannel(chan);
    
}

int net::EvLoop::delayCallbackCb(std::shared_ptr<net::Channel> chan){
    
    char delayBuff[1];
    read(m_delayCbReadFd, delayBuff, 1);
    
    m_hasSetDelayCb = false;
    
    return 0;
}























