//
//  ProtoClient.cpp
//  marstcp
//
//  Created by meryn on 2017/09/25.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "ProtoClient.h"
#include "net/Connector.h"
#include "ProtobufDispatcher.h"
#include "ProtoConnectionExt.h"
#include "ProtoConfig.h"
#include "log/Log.h"

net::ProtoClient::ProtoClient(std::shared_ptr<MultiLoop> _mLoop, std::string _name, std::string _host, int _port)
:net::TcpClient(_mLoop, _host, _port, _name),
net::ProtoDispatcherModule(std::static_pointer_cast<ProtoConfig>(m_config))
{}

net::ProtoClient::ProtoClient(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config)
:net::TcpClient(_mLoop, _config),
net::ProtoDispatcherModule(std::static_pointer_cast<ProtoConfig>(_config))
{}


int net::ProtoClient::Start(bool single/* = true*/, int loopIdx/* = -1*/){
    
    this->SetReadCallback(std::bind(&ProtoClient::protoReadBuff, std::static_pointer_cast<ProtoClient>(shared_from_this()), std::placeholders::_1, std::placeholders::_2));
    
    return TcpClient::Start(single, loopIdx);
}

std::shared_ptr<net::Connector> net::ProtoClient::GetNewConnector(std::shared_ptr<EvLoop> _loop, std::string _ip, int _port, std::string _name){
    
    std::shared_ptr<net::Connector> conn = TcpClient::GetNewConnector(_loop, _ip, _port, _name);
    
    conn->SetExtModule(std::make_shared<ProtoConnectionExt>());
    
    return conn;
    
}



int net::ProtoClient::protoReadBuff(std::shared_ptr<net::Connection> _conn, std::shared_ptr<net::Buff> _buf){
    
    if(this->m_dispatcher->decodeProtoAndHandle(std::static_pointer_cast<ProtoClient>(shared_from_this()), _conn, _buf) != 0){
        ErrorLog <<"handle msg failed,"<<this->GetPrintName();
        return 0;
    }
    
    return 0;
}


