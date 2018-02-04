//
//  ProtoServer.hpp
//  marstcp
//
//  Created by meryn on 2017/09/19.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef ProtoServer_hpp
#define ProtoServer_hpp

#include "net/TcpServer.h"
#include "ProtoDispatcherModule.h"




namespace net {
    
    class Buff;
    class Connection;
    
    class ProtobufHandlerBase;
    class ProtobufDispatcher;
    
    class ProtoServer:public TcpServer, public ProtoDispatcherModule{
    public:
        ProtoServer(std::shared_ptr<MultiLoop> _mLoop, std::string _name, std::string _host, int _port);
        
        ProtoServer(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config);
        
        int Start(bool single = true, int loopIdx = -1);
        
        std::shared_ptr<Connection> GetNewConnection(std::shared_ptr<EvLoop> _loop, std::shared_ptr<Channel> _chan, std::string _addr, int _port);
        
    private:
        int protoReadBuff(std::shared_ptr<net::Connection> _conn, std::shared_ptr<net::Buff> _buf);
        
    protected:
        
    };    
    

    

    
}











#endif /* ProtoServer_hpp */








