//
//  Channel.cpp
//  marstcp
//
//  Created by meryn on 2017/08/02.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "Channel.h"
#include "EvLoop.h"


net::Channel::~Channel(){
    
}

int net::Channel::UpdateEvent(){
    
    return this->m_loop.lock()->UpdateChannel(shared_from_this());
}




