//
//  Singleton.h
//  marstcp
//
//  Created by meryn on 2017/09/29.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Singleton_h
#define Singleton_h

#include "common/Noncopyable.h"
#include <mutex>

namespace common {
    
    template<typename T>
    class Singleton: public Noncopyable{
        
    protected:
        static T *m_instance;
        
        Singleton(){
        }
        
        virtual void init(){
            
        }
        
        
    public:
        static T * GetInstance(){
            static std::once_flag oc;
            std::call_once(oc, [&]{ m_instance = new T(); m_instance->init(); });
            return m_instance;
        }

    };
    
    template<typename T>
    T* Singleton<T>::m_instance;
}


#endif /* Singleton_h */
