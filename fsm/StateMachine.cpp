//
//  StateMachine.cpp
//  marstcp
//
//  Created by meryn on 2017/09/28.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "StateMachine.h"
#include "StateConfig.h"
#include "State.h"
#include "net/Tcp.h"
#include "common/common.h"


fsm::StateMachine::StateMachine(std::shared_ptr<net::Tcp> _tcp, int32_t _initState, uint32_t _id, std::shared_ptr<StateConfig> _config)
:m_id(_id), m_currState(_initState), m_isRunning(true), m_tcp(_tcp)
{
    if(_config.get() == nullptr){
        m_config = std::make_shared<StateConfig>();
    }    
}


std::shared_ptr<fsm::State> fsm::StateMachine::Config(int32_t _state, std::string _desc/*=""*/){
    
    return m_config->Config(_state, _desc);
    
}

std::string fsm::StateMachine::GetCurrStateName(){
    
    std::shared_ptr<State> state = getState(m_currState);
    
    if(state.get() == nullptr){
        char buff [10];
        snprintf(buff, sizeof(buff) , "State%d", state->GetId());
        return buff;
    }
    
    return state->GetName();
    
}


std::shared_ptr<fsm::State> fsm::StateMachine::getState(int32_t _state){
    
    return m_config->GetState(_state);
    
}


int fsm::StateMachine::exitState(std::shared_ptr<fsm::State> _state){
    
    if(_state.get() == nullptr){
        ErrorLog <<"state obj null, curr:"<<m_currState;
        return -1;
    }

    return _state->exit(shared_from_this());
}




int fsm::StateMachine::enterState(std::shared_ptr<fsm::State> _state){
    
    if(_state.get() == nullptr){
        ErrorLog <<"state obj null, curr:"<<m_currState;
        return -1;
    }
    
    return _state->enter(shared_from_this());
    
}



fsm::JumpRes fsm::StateMachine::getNextState(std::shared_ptr<fsm::State> _state, int32_t tid){
    
    return _state->getTriggerDstId(tid);
}


void fsm::StateMachine::finish(int32_t _err){
    ErrorLog <<"finish child not implemented:"<<m_id;
    
}


void fsm::StateMachine::reset(){
    
    m_currState = m_initState;
    
}


int fsm::StateMachine::fireInternal(int32_t _trigger, std::function<int()> _tFunc){
    
    bool ok = false;
    
    do{
        std::shared_ptr<fsm::State> state = getState(m_currState);
        
        if(state.get() == nullptr){
            ErrorLog <<"state is not registered:"<<m_currState;
            break;
        }
        
        auto transRet = getNextState(state, _trigger);
        
        if(transRet.ret < 0){
            ErrorLog <<"step to next state failed, settins err curr state:"<<m_currState << " tid:"<<_trigger;
            break;
        }
        
        if(transRet.m_isReentry){
            
            if(_tFunc() < 0){
                ErrorLog <<"trigger func failed" << _trigger;
                break;
            }
            
        }else{
        
            if(exitState(state) < 0){
                ErrorLog <<"exit state failed"<<m_currState;
                break;
            }
            
            if(_tFunc() < 0){
                ErrorLog <<"trigger func failed" << _trigger;
                break;
            }
            
            std::shared_ptr<fsm::State> dstate = getState(transRet.m_dstState);
            
            if(dstate.get() == nullptr){
                ErrorLog <<"dst state is not registered:"<<transRet.m_dstState;
                break;
            }
            
            if(enterState(dstate) < 0){
                ErrorLog <<"enter state failed"<<transRet.m_dstState;
                break;
            }
            
            m_currState = transRet.m_dstState;
            
            if(dstate->m_tickFunc){
                
                m_tcp->SetInterval(0, common::FILE_TRANSFER_INIT_DURATION_MS, dstate->m_tickFunc);
                
            }
        }
        
        ok = true;
    }while (0);
    
    if(!ok){
        
        Finish(-1);
        return -1;
    }
    
    return 0;
    
}

















