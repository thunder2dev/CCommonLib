//
//  StateMachine.hpp
//  marstcp
//
//  Created by meryn on 2017/09/28.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef StateMachine_hpp
#define StateMachine_hpp

#include "log/Log.h"
#include "common/Noncopyable.h"

#include <memory>
#include <string>

#include "StateMachineCommon.h"
#include "TriggerWithParameter.h"

namespace net{
    
    class Tcp;
    
}


namespace fsm {
    class State;
    class StateConfig;
    class JumpRes;
    
    class StateMachine:public common::Noncopyable, public std::enable_shared_from_this<StateMachine>{
        
    public:
        StateMachine(std::shared_ptr<net::Tcp> _tcp, int32_t _initState, uint32_t _id, std::shared_ptr<StateConfig> _config = nullptr);
        
        uint32_t GetId(){ return m_id; }
        
        void SetResetHandler(HelperHandler _handler){ m_resetHandler = _handler;}
        void SetFinishHandler(FinishHandler _handler){ m_finishHandler = _handler; }
        
        std::shared_ptr<State> Config(int32_t _state, std::string _desc="");
        
        int32_t GetCurrState(){ return m_currState; }
        std::string GetCurrStateName();
        
        int Fire(int32_t _trigger){
            return fireInternal(_trigger, []()->int{return 0;});
        }
        
        template<typename T, typename ...Args, int(T::*mf)(Args...)>
        int Fire(std::shared_ptr<TriggerWithParameter<int (T::*)(Args...), mf>> _trigger, std::shared_ptr<T> obj, Args... arg){
            return fireInternal(_trigger->GetTrigger(), [&]()->int{ return _trigger->call(obj, std::forward<Args>(arg)...); });
        }
        
        int Finish(int err){
            reset();
            finish(err);
            
            m_isRunning = false;
            
            if(m_finishHandler){
                m_finishHandler(err);
            }
            return err;
        }
        
        
    protected:
        int exitState(std::shared_ptr<fsm::State> _state);
        int enterState(std::shared_ptr<fsm::State> _state);
        
        
        int fireInternal(int32_t _trigger, std::function<int()> _tFunc);
        
        JumpRes getNextState(std::shared_ptr<fsm::State> _state, int32_t _tid);
        
        virtual void finish(int32_t _err);
        void reset();
        
        std::shared_ptr<State> getState(int32_t _state);
        
        void test(){
            InfoLog <<"ptr cnt:" << shared_from_this().use_count();
        }
        
        
    protected:
        
        uint32_t m_id;
        bool m_isRunning;
        
        std::shared_ptr<StateConfig> m_config;
        
        std::shared_ptr<net::Tcp> m_tcp;
        
        int32_t m_initState;
        int32_t m_currState;
        
        FinishHandler m_finishHandler;
        HelperHandler m_resetHandler;
        
    };
}





#endif /* StateMachine_hpp */
