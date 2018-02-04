//
//  Noncopyable.h
//  marstcp
//
//  Created by meryn on 2017/08/23.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Noncopyable_h
#define Noncopyable_h

namespace common {
    class Noncopyable{
    
    public:
        Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&) = delete;
        
        Noncopyable(Noncopyable&&) = delete;
        Noncopyable& operator=(Noncopyable&&) = delete;
    
        Noncopyable(){};
    
    };
}






#endif /* Noncopyable_h */
