//
//  RWLock.hpp
//  marstcp
//
//  Created by meryn on 2017/08/21.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef RWLock_hpp
#define RWLock_hpp



#include <pthread.h>
#include "Raii.h"

namespace common {
    
    class RWLock{
        
    public:
        RWLock();
        RWLock(const RWLock&) = delete;
        RWLock& operator=(const RWLock&) = delete;
        
        void RLock();
        void RUnlock();
        
        void WLock();
        void WUnlock();
        
        raii read_guard() noexcept{
            return make_raii(*this, &RWLock::RLock, &RWLock::RUnlock);
        }
        
        raii write_guard() noexcept{
            return make_raii(*this, &RWLock::WLock, &RWLock::WUnlock);
        }
        
        
    private:
        pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
    };
}

#endif /* RWLock_hpp */













