//
//  State.cpp
//  marstcp
//
//  Created by meryn on 2017/09/28.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "State.h"
#include "StateTrans.h"

#include "log/Log.h"


fsm::State::State(int32_t _sid, std::string _name /* ="" */)
:m_sid(_sid),
m_name(_name)
{
    
}



fsm::State* fsm::State::Permit(int32_t _dstSid, int32_t _trigger){
    assert(m_transAll.count(_trigger) == 0);
    assert(m_sid != _dstSid);
    
    std::shared_ptr<StateTrans> trans = std::make_shared<StateTrans>(m_sid, _dstSid, _trigger, false);
    
    m_transAll[_trigger] = trans;
    
    return this;
}

fsm::State* fsm::State::PermitReentry(int32_t _trigger){
    assert(m_transAll.count(_trigger) == 0);
    
    std::shared_ptr<StateTrans> trans = std::make_shared<StateTrans>(m_sid, m_sid, _trigger, true);
    
    m_transAll[_trigger] = trans;
    
    return this;
}

fsm::JumpRes fsm::State::getTriggerDstId(int32_t _tid){
    
    JumpRes res;
    
    if(m_transAll.count(_tid) <= 0){
        ErrorLog <<"trans not registered";
        res.ret = -1;
        return res;
    }
    
    std::shared_ptr<StateTrans> trans = m_transAll[_tid];
    if(trans.get() == nullptr){
        ErrorLog <<"trans null";
        res.ret = -1;
        return res;
    }
    
    if((trans->GetDstId() == trans->GetSrcId()) && !trans->GetReentry()){
        ErrorLog <<"trans is not reentry, state id:" <<trans->GetDstId();
        res.ret = -1;
        return res;
    }
    
    InfoLog <<"src:"<<trans->GetSrcId() <<" trigger:"<< _tid <<" dst:"<<trans->GetDstId();
    
    res.ret = 0;
    res.m_dstState = trans->GetDstId();
    res.m_isReentry = trans->GetReentry();
    
    return res;
    
}

bool fsm::State::IsTriggerReentry(int32_t _trigger){
    
    if(m_transAll.count(_trigger) <= 0){
        ErrorLog <<"trans not registered";
        return false;
    }
    
    std::shared_ptr<StateTrans> trans = m_transAll[_trigger];
    if(trans.get() == nullptr){
        ErrorLog <<"trans null";
        return false;
    }
    
    return trans->GetReentry();
    
}















