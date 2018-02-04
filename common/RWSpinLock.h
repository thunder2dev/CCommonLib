//
//  RWLock.hpp
//  marstcp
//
//  Created by meryn on 2017/08/18.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef RWSpinLock_hpp
#define RWSpinLock_hpp


#include <atomic>
#include <thread>
#include "Raii.h"

namespace common {

class RWSpinLock{
    
public:
    RWSpinLock(const RWSpinLock&) = delete;
    RWSpinLock& operator=(const RWSpinLock&) = delete;
    
    RWSpinLock(bool _isWriteFirst = true);
    
    void RLock();
    void RUnlock();
    
    void WLock();
    void WUnlock();
    
    raii read_guard() noexcept{
        return make_raii(*this, &RWSpinLock::RLock, &RWSpinLock::RUnlock);
    }
    
    raii write_guard() noexcept{
        return make_raii(*this, &RWSpinLock::WLock, &RWSpinLock::WUnlock);
    }
    
private:
    
    static std::thread::id invalid_pid;
    
    std::thread::id write_pid;
    
    bool isWriteFirst;
    
    std::atomic_int lockCount;  // <0: w lock, >0: r lock
    std::atomic_int waitCount;
};




}



#endif /* RWLock_hpp */









