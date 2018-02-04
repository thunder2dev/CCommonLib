//
//  EvLoop.hpp
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef EvLoop_hpp
#define EvLoop_hpp

#include <memory>
#include <thread>
#include "common/callbacks.h"
#include "common/MQueue.h"


namespace common {
    class TimerEvent;
    class TimeQueue;
}

namespace net {

class MPoll;
class Channel;


class EvLoop: public std::enable_shared_from_this<EvLoop>{     //each loop is not async
    
public:
    typedef std::shared_ptr<EvLoop> Ptr;
    
    EvLoop(int _loopIdx);
    
    void Run();
    void Loop();
    
    int AddChannel(std::shared_ptr<Channel> chan);
    int DelChannel(std::shared_ptr<Channel> chan);
    int UpdateChannel(std::shared_ptr<Channel> chan);
    void AddInLoopJob(std::function<common::DelayCallback> delayCB);
    bool IsInLoopThread();
    
    void SetTimeOut(int _milli, common::TimeoutCallback _cb);
    void SetInterval(int _offset, int _intval, common::TimeoutCallback _cb);
    
    
private:
    
    int addChannelInLoop(std::shared_ptr<Channel> chan);
    int updateChannelInLoop(std::shared_ptr<Channel> chan);
    int delChannelInLoop(std::shared_ptr<Channel> chan);
    
    int handleDelayJobs();
    int delayCallbackCb(std::shared_ptr<net::Channel> chan);
    
    void setupTimer();
    void setupDelayCallbackFds();
    
    void setTimeOutInLoop(int _milli, common::TimeoutCallback _cb);
    void setIntervalInLoop(int _offset, int _intval, common::TimeoutCallback _cb);
    
    
private:
    std::shared_ptr<MPoll> m_poll;
    std::shared_ptr<common::TimeQueue> m_timeQueue;
    common::MQueue<std::function<common::DelayCallback>> m_delayCallbacks;
    
    
    int m_loopIdx;
    std::thread::id m_tid;
    bool m_isRunning;
    
    int m_delayCbReadFd, m_delayCbWriteFd;
    std::atomic<bool> m_hasSetDelayCb;
    
    
    static const std::thread::id NULL_THREAD_ID;
    
};

}
    
#endif /* EvLoop_hpp */





