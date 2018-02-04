//
//  TriggerWithParameter.h
//  marstcp
//
//  Created by meryn on 2017/09/29.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef TriggerWithParameter_h
#define TriggerWithParameter_h

#include "SMFunc.h"

namespace fsm {
    
    template <typename T, T t> class TriggerWithParameter;
    
    template<typename T, typename ...Args, int(T::*mf)(Args...)>
    class TriggerWithParameter<int (T::*)(Args...), mf>: public SMFunc<int (T::*)(Args...), mf>{
    public:
        TriggerWithParameter(int32_t _trigger, std::string _name=""):SMFunc<int (T::*)(Args...), mf>(_name), m_trigger(_trigger){}
        int32_t GetTrigger(){ return m_trigger; }
        
    private:
        int32_t m_trigger;
    };
    

}



#endif /* TriggerWithParameter_h */
