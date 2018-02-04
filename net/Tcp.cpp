//
//  Tcp.cpp
//  marstcp
//
//  Created by meryn on 2017/09/26.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "Tcp.h"
#include "Connector.h"
#include "TcpConfig.h"
#include <cassert>
#include "EvLoop.h"

net::Tcp::Tcp(std::shared_ptr<MultiLoop> _mLoop, std::string _host, int _port, std::string _name)
:m_loops(_mLoop), m_host(_host), m_port(_port), m_name(_name), m_isSingleThread(true), m_loopIdx(-1)
{
}

net::Tcp::Tcp(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config)
:m_loops(_mLoop), m_config(_config)
{
    assert(m_config != nullptr);
    assert(m_config->Load() == 0);
    
    m_host = m_config->m_ip;
    m_port = m_config->m_port;
    m_name = m_config->m_name;
    
}




std::shared_ptr<net::Connector> net::Tcp::GetNewConnector(std::shared_ptr<EvLoop> _loop, std::string _ip, int _port, std::string _name){
    return std::make_shared<Connector>(m_mainLoop, _ip, _port, _name);
}

std::shared_ptr<net::Connection> net::Tcp::GetNewConnection(std::shared_ptr<EvLoop> _loop, std::shared_ptr<Channel> _chan, std::string _addr, int _port){
    return std::make_shared<Connection>(_loop, _chan, _addr, _port);
    
}


void net::Tcp::SetTimeOut(int _milli, common::TimeoutCallback _cb){
    
    this->GetMainLoop()->SetTimeOut(_milli, _cb);
    
}



void net::Tcp::SetInterval(int _offset, int _intval, common::TimeoutCallback _cb){
    
    this->GetMainLoop()->SetInterval(_offset, _intval, _cb);
    
}















