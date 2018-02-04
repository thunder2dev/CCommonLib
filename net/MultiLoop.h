//
//  MultiLoop.hpp
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef MultiLoop_hpp
#define MultiLoop_hpp

#include "common/callbacks.h"
#include "common/MVector.h"
#include "common/MQueue.h"
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <memory>

namespace net {

class EvLoop;

class MultiLoop: public common::Noncopyable, public std::enable_shared_from_this<MultiLoop>{

public:
    MultiLoop();
    
    MultiLoop(int nLoops);
    
    std::shared_ptr<MultiLoop> GetPtr(){ return shared_from_this(); }
    
    void AddLoop(const std::shared_ptr<EvLoop> loop);  //should not be called

    std::shared_ptr<EvLoop> GetAvailLoop();
    
    std::shared_ptr<EvLoop> GetLoopByIdx(int idx);
    
    void Run(bool block = true);
    
    
private:
    
    void AddLoopInMainThread(const std::shared_ptr<EvLoop> loop);
    
    void AppendToDelayCalls(std::function<common::DelayCallback> delayCb);
    
private:
    
    int nLoops;
    
    std::atomic<int> currloopIdx;
    
    common::MVector<std::shared_ptr<EvLoop>> m_loops;
    
    std::vector<std::thread> m_threads;
    
    common::MQueue<std::function<common::DelayCallback>> delayCallbacks;
};

}

#endif /* MultiLoop_hpp */














