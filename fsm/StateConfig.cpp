//
//  StateConfig.cpp
//  marstcp
//
//  Created by meryn on 2017/09/28.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "StateConfig.h"
#include "State.h"

std::shared_ptr<fsm::State> fsm::StateConfig::Config(int32_t _state, std::string _desc/*=""*/){
    
    assert(m_states.count(_state) == 0);
    
    std::shared_ptr<fsm::State> state = std::make_shared<fsm::State>(_state, _desc);
    
    m_states[_state] = state;
    
    return state;
    
}





