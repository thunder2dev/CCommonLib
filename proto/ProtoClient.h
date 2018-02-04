//
//  ProtoClient.hpp
//  marstcp
//
//  Created by meryn on 2017/09/25.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef ProtoClient_hpp
#define ProtoClient_hpp

#include "net/TcpClient.h"
#include "ProtoCommon.h"
#include "ProtoDispatcherModule.h"

namespace google {
    namespace protobuf{
        class Message;
    }
}


namespace net {
    
    class Buff;
    
    class ProtobufDispatcher;
    
    class ProtoClient:public TcpClient, public ProtoDispatcherModule{
    public:
        ProtoClient(std::shared_ptr<MultiLoop> _mLoop, std::string _name, std::string _host, int _port);
        
        ProtoClient(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config);
        
        int Start(bool single = true, int loopIdx = -1);
        
        std::shared_ptr<Connector> GetNewConnector(std::shared_ptr<EvLoop> _loop, std::string _ip, int _port, std::string _name);
        
    protected:
        
        int protoReadBuff(std::shared_ptr<net::Connection> _conn, std::shared_ptr<net::Buff> _buf);
        
        
        
    };
    
    
}




#endif /* ProtoClient_hpp */
