//
//  RWLock.cpp
//  marstcp
//
//  Created by meryn on 2017/08/18.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "RWSpinLock.h"
#include "log/Log.h"


namespace common{


RWSpinLock::RWSpinLock(bool _isWriteFirst):lockCount(0), waitCount(0), isWriteFirst(_isWriteFirst){
}


std::thread::id RWSpinLock::invalid_pid;


void RWSpinLock::RLock(){
    
    int count;
    
    do{
        while (( isWriteFirst && this->waitCount != 0) || (count = this->lockCount) < 0);     //not true read first until add readWaitCount
    }while (!this->lockCount.compare_exchange_weak(count, count+1));
    
}

void RWSpinLock::RUnlock(){
    
    assert(this->lockCount > 0);
    
    this->lockCount--;
    
}



void RWSpinLock::WLock(){
    
    this->waitCount++;
    
    for(int ii = 0; !this->lockCount.compare_exchange_weak(ii, ii-1); ii =0 );
    
    assert(this->lockCount == -1);
    
    this->waitCount--;
    
    this->write_pid = std::this_thread::get_id();
    
}


void RWSpinLock::WUnlock(){
    
    assert(std::this_thread::get_id() == this->write_pid);
    
    assert(this->lockCount == -1);
    
    this->write_pid = invalid_pid;

    this->lockCount++;  // this atomic must be last
}




}


