//
//  TcpClient.hpp
//  marstcp
//
//  Created by meryn on 2017/09/14.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef TcpClient_hpp
#define TcpClient_hpp

#include <memory>
#include <functional>
#include "callbacks.h"
#include "Tcp.h"


namespace net {
    
    class MultiLoop;
    class EvLoop;
    class Connector;
    
    
    class TcpClient: public Tcp{   //Tcp socket fd is in one loop( in one thread ), but read from many threads
        
    public:
        
        TcpClient(std::shared_ptr<MultiLoop> _mLoop, std::string _host, int _port, std::string _name = "");
        
        TcpClient(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config);
        
        int Start(bool single = true, int loopIdx = -1);
        
        void SetConnectecCallback(std::function<common::ConnectedCallback>&& _connectCallback){
            this->m_connectCallback = std::move(_connectCallback);
        }
        
        
    private:
        int tcpConnectCallback();
        int tcpCloseCallback(std::shared_ptr<Connection> con);
        
    private:
        
        std::function<common::ConnectedCallback> m_connectCallback;
        
        std::shared_ptr<Connector> m_connector;
        
        
        
    };
    
}


#endif /* TcpClient_hpp */
