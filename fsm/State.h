//
//  State.hpp
//  marstcp
//
//  Created by meryn on 2017/09/28.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef State_hpp
#define State_hpp

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cassert>

#include "common/callbacks.h"
#include "StateMachineCommon.h"
#include "TriggerWithParameter.h"

namespace fsm {
    
    class StateMachine;
    class StateTrans;
    
    struct JumpRes{
        int32_t m_dstState;
        bool m_isReentry;
        int ret;
        
    };
    
    
    class State{
    public:
        State(int32_t _sid, std::string _name="");
        
        int32_t GetId() { return m_sid; }
        std::string GetName() { return m_name; }
        
        State* OnEntry(BehaviourFunc _func){ m_entrieFuncs.push_back(_func); return this; }
        State* OnExit(BehaviourFunc _func){ m_exitFuncs.push_back(_func); return this; }
        State* Permit(int32_t _dstSid, int32_t _trigger);
        State* PermitReentry(int32_t _trigger);
        State* Tick(common::TimeoutCallback _func){ m_tickFunc = _func; return this; };
        
        bool IsTriggerReentry(int32_t _trigger);
        
        template<typename T, typename ...Args, int(T::*mf)(Args...)>
        State* Permit(int32_t _dstSid, std::shared_ptr<TriggerWithParameter<int (T::*)(Args...), mf>> _trigger){
            return Permit(_dstSid, _trigger->GetTrigger());
        }
        
        template<typename T, typename ...Args, int(T::*mf)(Args...)>
        State* PermitReentry(std::shared_ptr<TriggerWithParameter<int (T::*)(Args...), mf>> _trigger){
            return PermitReentry(_trigger->GetTrigger());
        }
        
    private:
        
        friend StateMachine;
        
        int exit(std::shared_ptr<StateMachine> _sm){
            for(const BehaviourFunc& func: m_exitFuncs){
                if(func() < 0){
                    ErrorLog <<"exit func err:";
                    return -1;
                }
            }
            return 0;
        }
        
        int enter(std::shared_ptr<StateMachine> _sm){
            for(const BehaviourFunc& func: m_entrieFuncs){
                if(func() < 0){
                    ErrorLog <<"entry func err:";
                    return -1;
                }                
            }
            return 0;
        }
        
        JumpRes getTriggerDstId(int32_t _tid);
        
    private:
        int32_t m_sid;
        std::string m_name;
        std::vector<BehaviourFunc> m_entrieFuncs;
        std::vector<BehaviourFunc> m_exitFuncs;

        common::TimeoutCallback m_tickFunc;
        
        std::map<int32_t, std::shared_ptr<StateTrans>> m_transAll; //trigger as key
        
        
    };

}







#endif /* State_hpp */
