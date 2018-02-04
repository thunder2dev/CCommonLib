//
//  Poll.hpp
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Poll_hpp
#define Poll_hpp


#include "common/callbacks.h"
#include <map>

namespace net {


class Channel;

class MPoll: public std::enable_shared_from_this<MPoll>{   //one poll in one loop to be sync
    
public:
    virtual ~MPoll(){};
    
    virtual int AddChannel(std::shared_ptr<Channel>& chan) = 0;
    virtual int DelChannel(std::shared_ptr<Channel>& chan) = 0;
    
    virtual int UpdateEvent(std::shared_ptr<Channel>& chan) = 0;
    
    virtual int Poll(int waitms) = 0;
    
protected:
    int pollFd;
    std::map<int, std::shared_ptr<Channel>> channels;
};

}
    
#endif /* Poll_hpp */
