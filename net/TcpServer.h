//
//  TcpServer.hpp
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef TcpServer_hpp
#define TcpServer_hpp

#include <memory>
#include <functional>
#include "common/callbacks.h"
#include "common/MMap.h"
#include "Tcp.h"

namespace net {

class MultiLoop;


class TcpServer: public Tcp{   //Tcp socket fd is in one loop( in one thread ), but read from many threads
    
public:
    
    TcpServer(std::shared_ptr<MultiLoop> _mLoop, std::string _host, int _port, std::string _name);
    
    TcpServer(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config);
    
    int Start(bool single = true, int loopIdx = -1);
    
    
    void SetAcceptCallback(std::function<common::AcceptCallback>&& _acceptCallback){
        this->m_acceptCallback = std::move(_acceptCallback);
    }
        
private:
    
    int pollReadCallback(std::shared_ptr<net::Channel> chan);
    int tcpAcceptCallback();
    int tcpCloseCallback(std::shared_ptr<Connection> con);
    
    
private:
    
    std::function<common::AcceptCallback> m_acceptCallback;
    
    common::MMap<int, std::shared_ptr<Connection>> m_connections;
    
    
};

}



#endif /* TcpServer_hpp */
