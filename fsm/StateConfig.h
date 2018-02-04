//
//  StateConfig.hpp
//  marstcp
//
//  Created by meryn on 2017/09/28.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef StateConfig_hpp
#define StateConfig_hpp

#include <memory>
#include <string>
#include <map>
//#include "common/Singleton.h"



namespace fsm {
    
    class State;
    
    class StateConfig/*: public common::Singleton<StateConfig>*/{
    private:
        //friend common::Singleton<StateConfig>;
        //StateConfig(){}
        void init(){
        }
        
    public:
        
        std::shared_ptr<State> Config(int32_t _state, std::string _desc="");
        
        std::shared_ptr<State> GetState(int32_t _state){
            if(m_states.count(_state) <= 0){
                return nullptr;
            }
            return m_states[_state];
        }
        
        
        
    private:
        
        bool hasState(int32_t _state){ return m_states.count(_state); }
        
    private:
        
        std::map<int32_t, std::shared_ptr<State>> m_states;
        
        
        
    };
}





#endif /* StateConfig_hpp */
