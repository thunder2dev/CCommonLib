//
//  ProtoServer.cpp
//  marstcp
//
//  Created by meryn on 2017/09/19.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "ProtoServer.h"
#include "ProtobufDispatcher.h"
#include "ProtoConnectionExt.h"
#include "ProtoConfig.h"

#include "net/Connection.h"
//#include "common/Log.h"
#include "net/Buff.h"

net::ProtoServer::ProtoServer(std::shared_ptr<MultiLoop> _mLoop, std::string _name, std::string _host, int _port)
:TcpServer(_mLoop, _host, _port, _name),
ProtoDispatcherModule(std::static_pointer_cast<ProtoConfig>(m_config))
{
}

net::ProtoServer::ProtoServer(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config)
:net::TcpServer(_mLoop, _config),
ProtoDispatcherModule(std::static_pointer_cast<ProtoConfig>(_config))
{}


std::shared_ptr<net::Connection> net::ProtoServer::GetNewConnection(std::shared_ptr<EvLoop> _loop, std::shared_ptr<Channel> _chan, std::string _addr, int _port){
    
    std::shared_ptr<Connection> conn = TcpServer::GetNewConnection(_loop, _chan, _addr, _port);
    conn->SetExtModule(std::make_shared<ProtoConnectionExt>());
    
    return conn;
}


int net::ProtoServer::Start(bool single/* = true*/, int loopIdx/* = -1*/){
    
    this->SetReadCallback(std::bind(&ProtoServer::protoReadBuff, std::static_pointer_cast<ProtoServer>(shared_from_this()), std::placeholders::_1, std::placeholders::_2));
    
    return TcpServer::Start(single, loopIdx);
}



int net::ProtoServer::protoReadBuff(std::shared_ptr<net::Connection> _conn, std::shared_ptr<net::Buff> _buf){
  
  if(this->m_dispatcher->decodeProtoAndHandle(std::static_pointer_cast<ProtoServer>(shared_from_this()), _conn, _buf) != 0){
      ErrorLog <<"handle msg failed,"<<this->GetPrintName();
      return 0;
  }
  
  return 0;
}














