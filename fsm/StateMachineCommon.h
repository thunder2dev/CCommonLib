//
//  StateMachineCommon.h
//  marstcp
//
//  Created by meryn on 2017/09/29.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef StateMachineCommon_h
#define StateMachineCommon_h


namespace fsm {
    
    const int32_t StateError = -1;
    const int32_t StateFinish = -2;
    
    
    class StateMachine;
    typedef std::function<int(std::shared_ptr<StateMachine>)> BehaviourFuncNeedSM;
    typedef std::function<int()> BehaviourFunc;
    typedef std::function<int()> HelperHandler;
    typedef std::function<void(int)> FinishHandler;
#define emptyHandler [](){}
    
}





#endif /* StateMachineCommon_h */
