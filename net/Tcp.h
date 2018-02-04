//
//  Tcp.hpp
//  marstcp
//
//  Created by meryn on 2017/09/26.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Tcp_hpp
#define Tcp_hpp

#include <memory>
#include <string>
#include <functional>
#include "common/callbacks.h"


namespace net {
    
    class MultiLoop;
    class EvLoop;
    class Connection;
    class Connector;
    class TcpConfig;
    
    class Tcp: public std::enable_shared_from_this<Tcp>{        
        
    public:
        
        Tcp(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config);
        
        Tcp(std::shared_ptr<MultiLoop> _mLoop, std::string _host, int _port, std::string _name = "");        
        
        virtual int Start(bool single = true, int loopIdx = -1) = 0;
        
        void SetReadCallback(std::function<common::TcpReadCallback>&& _readCallback){
            this->m_readCallback = std::move(_readCallback);
        }
        
        void SetWriteCallback(std::function<common::PollWriteCallback> _writeCallback){
            this->m_writeCallback = std::move(_writeCallback);
        }
        
        void SetConnectionCloseCallback(common::ConnectionCloseCallback _closeCallback){ m_closeCallback = _closeCallback; }
        void SetServerStartCallback(common::ServerStartCallback _startCallback) { m_serverStartCallback = _startCallback; }
        
        std::string GetPrintName(){
            return m_name + ":" + std::to_string(m_port);
        }
        
        std::shared_ptr<EvLoop> GetMainLoop(){
            return m_mainLoop;
        }
        
        std::shared_ptr<MultiLoop> GetMultiLoops(){
            return m_loops;
        }
        
        virtual std::shared_ptr<Connection> GetNewConnection(std::shared_ptr<EvLoop> _loop, std::shared_ptr<Channel> _chan, std::string _addr, int _port);
        virtual std::shared_ptr<Connector> GetNewConnector(std::shared_ptr<EvLoop> _loop, std::string _ip, int _port, std::string _name);
        
        void SetTimeOut(int _milli, common::TimeoutCallback _cb);
        void SetInterval(int _offset, int _intval, common::TimeoutCallback _cb);
        
        
    protected:
        
        int pollReadCallback(std::shared_ptr<net::Channel> chan);
        
        
        int tcpAcceptCallback();
        int tcpCloseCallback(std::shared_ptr<Connection> con);
        
        
    protected:
        int m_socketFd;
        
        bool m_isSingleThread;
        int m_loopIdx;
        
        std::shared_ptr<TcpConfig> m_config;
        
        std::string m_name;
        std::string m_host;
        int m_port;
        
        std::shared_ptr<MultiLoop> m_loops;
        std::shared_ptr<EvLoop> m_mainLoop;
        
        common::ServerStartCallback m_serverStartCallback;
        
        std::function<common::TcpReadCallback> m_readCallback;
        std::function<common::PollWriteCallback> m_writeCallback;
        common::ConnectionCloseCallback m_closeCallback;
        
        
        
    };
}
















#endif /* Tcp_hpp */
