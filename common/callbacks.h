//
//  callbacks.h
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef callbacks_h
#define callbacks_h

#include <memory>
#include <functional>

namespace net {
    class Channel;
    class Connection;
    class Buff;
    class Tcp;
}


namespace common {
    
    class TimerEvent;
    
    typedef void DelayCallback();
    
    typedef int PollReadCallback(std::shared_ptr<net::Channel> chan);
    typedef int PollWriteCallback(std::shared_ptr<net::Channel> chan);
    typedef int AcceptCallback(std::shared_ptr<net::Connection> con);
    typedef int TcpReadCallback(std::shared_ptr<net::Connection> _conn, std::shared_ptr<net::Buff> _buf);
    //typedef int TcpWriteCallback(std::shared_ptr<net::Buff> _buff);
    
    
    typedef int ConnectedCallback(std::shared_ptr<net::Connection> con);
    
    typedef std::function<int()> ServerStartCallback;
    typedef std::function<int(std::shared_ptr<net::Connection> con)> ConnectionCloseCallback;

    typedef std::function<int(std::shared_ptr<TimerEvent> _ev)> TimeoutCallback;
    
    typedef std::function<int(int)> CommonRetCallback;
}




#endif /* callbacks_h */
