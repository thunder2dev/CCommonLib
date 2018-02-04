//
//  Poll.hpp
//  marstcp
//
//  Created by meryn on 2017/08/02.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef KQueue_Poll_hpp
#define KQueue_Poll_hpp

#include <stdio.h>
#include "common/common.h"
#include <sys/event.h>
#include <map>
#include <memory>
#include "MPoll.h"

namespace net {

class Channel;

class KQueuePoll :public MPoll{
public:
    KQueuePoll();
    ~KQueuePoll(){};
    
    int AddChannel(std::shared_ptr<Channel>& chan);
    int UpdateEvent(std::shared_ptr<Channel>& chan);
    int DelChannel(std::shared_ptr<Channel>& chan);
    
    
    int Poll(int waitms);
    

private:
    struct kevent ev[common::MAX_POLL_WAIT_EVENTS_NUM];

};

}
    
#endif /* Poll_hpp */






























