//
//  MultiLoop.cpp
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "MultiLoop.h"
#include "EvLoop.h"


net::MultiLoop::MultiLoop():MultiLoop(1){
    
}

net::MultiLoop::MultiLoop(int _nLoops): nLoops(_nLoops), currloopIdx(0){

    for(int ii = 0; ii < nLoops; ii++){
        this->m_loops.push_back(std::make_shared<EvLoop>(ii));
    }    
}


void net::MultiLoop::AddLoop(const std::shared_ptr<EvLoop> loop){
    
    this->AppendToDelayCalls(std::bind(&MultiLoop::AddLoopInMainThread, this->GetPtr(), loop));
    
}

void net::MultiLoop::AddLoopInMainThread(const std::shared_ptr<EvLoop> loop){
    
    
}


void net::MultiLoop::AppendToDelayCalls(std::function<common::DelayCallback> delayCb){
    this->delayCallbacks.Enqueue(std::move(delayCb));
}


std::shared_ptr<net::EvLoop> net::MultiLoop::GetAvailLoop(){
    
    int currIdx;
    int newIdx;
    
    do{
        currIdx = this->currloopIdx;
        newIdx = (currIdx + 1)%this->nLoops;
    }while (!this->currloopIdx.compare_exchange_weak(currIdx, newIdx));
    
    return this->m_loops.get(currIdx);
}


void net::MultiLoop::Run(bool block){
    
    this->m_loops.for_each_read([this](const std::shared_ptr<EvLoop>& loop){
        this->m_threads.push_back(std::thread(std::bind(&EvLoop::Run, loop)));
    });

    if(block){
        for(auto& th: this->m_threads) th.join();
    }
}

std::shared_ptr<net::EvLoop> net::MultiLoop::GetLoopByIdx(int idx){
    
    if(m_loops.size() <= idx){
        ErrorLog <<"loop idx err:"<<idx <<" loop size:"<<m_loops.size();
        return nullptr;
    }
    
    return GetLoopByIdx(idx);
}


































