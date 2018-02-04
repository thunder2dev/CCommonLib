//
//  TcpClient.cpp
//  marstcp
//
//  Created by meryn on 2017/09/14.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "TcpClient.h"
#include "Connector.h"
#include "MultiLoop.h"


net::TcpClient::TcpClient(std::shared_ptr<MultiLoop> _mLoop, std::string _ip, int _port, std::string _name /*= ""*/)
:net::Tcp(_mLoop, _ip, _port, _name)
{
}

net::TcpClient::TcpClient(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config)
:net::Tcp(_mLoop, _config)
{
}

int net::TcpClient::tcpCloseCallback(std::shared_ptr<Connection> con){
    
    if(m_closeCallback){
        m_closeCallback(con);
    }
    
    m_connector = nullptr;
    
    InfoLog <<"local client down: "<<con->GetRemoteAddr() << " :" << con->GetRemotePort();
    
    return 0;
}


int net::TcpClient::Start(bool single/* = true*/, int loopIdx/* = -1*/){
    
    m_isSingleThread = single;
    m_loopIdx = loopIdx;
    
    m_mainLoop = nullptr;
    
    if(m_isSingleThread && m_loopIdx >= 0){
        m_mainLoop = m_loops->GetLoopByIdx(m_loopIdx);
    }else{
        m_mainLoop = this->m_loops->GetAvailLoop();
    }

    if(m_mainLoop.get() == nullptr){
        ErrorLog <<"get avail loop for client failed "<<GetPrintName();
        exit(EXIT_FAILURE);
    }
    
    m_connector = GetNewConnector(m_mainLoop, m_host, m_port, m_name);
    
    m_connector->SetConnectedCallback(std::bind(&net::TcpClient::tcpConnectCallback, std::static_pointer_cast<TcpClient>(shared_from_this())));
    m_connector->SetReadCallback(m_readCallback);
    m_connector->SetCloseCallback(std::bind(&net::TcpClient::tcpCloseCallback, this, std::placeholders::_1));
    
    m_connector->Connect();
    
    
    if(m_serverStartCallback){
        m_serverStartCallback();
    }
    
    return 0;
}

int net::TcpClient::tcpConnectCallback(){
    
    InfoLog <<"connected:"<< m_host << " :" << m_port <<" fd:"<<m_connector->GetFd();
   
    std::shared_ptr<Connection> conn = std::static_pointer_cast<Connection>(m_connector);
    
    if(m_connectCallback){
        m_connectCallback(conn);
    }
    
    
    return 0;
}



