//
//  RWLock.cpp
//  marstcp
//
//  Created by meryn on 2017/08/21.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "RWLock.h"

namespace common {
    RWLock::RWLock(){
    }
    
    void RWLock::RLock(){
        pthread_rwlock_rdlock(&rwlock);
    }
    
    void RWLock::RUnlock(){
        pthread_rwlock_unlock(&rwlock);
    }
    
    void RWLock::WLock(){
        pthread_rwlock_wrlock(&rwlock);
    }
    
    void RWLock::WUnlock(){
        pthread_rwlock_unlock(&rwlock);
    }
}














