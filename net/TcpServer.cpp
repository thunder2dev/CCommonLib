//
//  TcpServer.cpp
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "TcpServer.h"

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>

#include "EvLoop.h"
#include "MultiLoop.h"
#include "Connection.h"
#include "Channel.h"
#include "Buff.h"
#include "common/MMap.h"
#include "SocketUtil.h"
#include "common/FileUtil.h"

net::TcpServer::TcpServer(std::shared_ptr<MultiLoop> _mLoop, std::string _host, int _port, std::string _name)
:net::Tcp(_mLoop, _host, _port, _name)
{    
}

net::TcpServer::TcpServer(std::shared_ptr<MultiLoop> _mLoop, std::shared_ptr<TcpConfig> _config)
:net::Tcp(_mLoop, _config)
{}



int net::TcpServer::Start(bool single/* = true*/, int loopIdx/* = -1*/){
    
    m_isSingleThread = single;
    m_loopIdx = loopIdx;
    
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons(m_port);
    
    if((this->m_socketFd=socket(PF_INET,SOCK_STREAM,0))<0)
    {
        FatalLog <<"socket create failed. " << m_name <<" port:" <<m_port <<" err:"<<strerror(errno);
        exit(-1);
    }
    
    ::setsockopt(this->m_socketFd, SOL_SOCKET, SO_REUSEADDR, &addr, sizeof(addr));
    
    
    if(common::setNonBlocking(this->m_socketFd) < 0){
        ErrorLog <<"server nonblocking failed:"<<this->GetPrintName();
        exit(EXIT_FAILURE);
    }
    
    if(common::setClosExec(this->m_socketFd) < 0){
        ErrorLog <<"server closexec failed:"<<this->GetPrintName();
        exit(EXIT_FAILURE);
    }
    
    
    if (bind(this->m_socketFd,(struct sockaddr *)&addr,sizeof(struct sockaddr))<0)
    {
        FatalLog <<"socket bind failed. " << m_name <<" port:" <<m_port<<" err:"<<strerror(errno);
        exit(-1);
    }
    
    if(listen(this->m_socketFd, 128)<0){
        FatalLog <<"socket listen failed. " << m_name <<" port:" <<m_port<<" err:"<<strerror(errno);
        exit(-1);
    }
    
    EvLoop::Ptr loop = nullptr;
    
    if(m_isSingleThread && m_loopIdx >= 0){
        loop = m_loops->GetLoopByIdx(m_loopIdx);
    }else{
        loop = this->m_loops->GetAvailLoop();
    }

    if(loop.get() == nullptr){
        ErrorLog <<"get avail loop failed "<<GetPrintName();
        exit(EXIT_FAILURE);
    }
    
    this->m_mainLoop = loop;
    
    Channel::Ptr chan = std::make_shared<Channel>(m_socketFd, loop);
    chan->SetReadCallback(std::bind(&TcpServer::pollReadCallback, this, chan));
    chan->EnableRead();
    chan->UpdateEvent();
    
    loop->AddChannel(chan);
    
    InfoLog <<"server " <<m_name <<" started:" <<m_port <<" fd:"<<m_socketFd;
    
    
    if(m_serverStartCallback){
        m_serverStartCallback();
    }
    
    return 0;
}



int net::TcpServer::pollReadCallback(std::shared_ptr<net::Channel> chan){
    if(chan->GetFd() == this->m_socketFd){
        return this->tcpAcceptCallback();
    }else{
        ErrorLog << "poll fd and listening fd mismatched";
        return -1;
    }
}


int net::TcpServer::tcpAcceptCallback(){
    
    int client_fd;
    struct sockaddr_in remote_addr;
    socklen_t sin_size;
    
    if((client_fd=::accept(this->m_socketFd, (struct sockaddr *)&remote_addr, &sin_size)) < 0){
        if (errno != EAGAIN && errno != EWOULDBLOCK){
            ErrorLog <<"accept failed:" << std::strerror(errno);
            return -1;
        }
    }
    
    char hostBuf[NI_MAXHOST];
    
    int port = ntohs(remote_addr.sin_port);
    
    if(getnameinfo((struct sockaddr *)&remote_addr, sin_size, hostBuf, sizeof hostBuf, NULL, 0, NI_NUMERICSERV | NI_NUMERICHOST) < 0){
        ErrorLog <<"get client addr failed:<"<<std::strerror(errno);
        return -1;
    }
    
    if(common::setNonBlocking(client_fd) < 0){
        FatalLog <<"set client nonblocing failed"<<hostBuf << " :" << port;
        close(client_fd);
        return -1;
    }
    
    int recvLowWaterNum = kBuffHeadSize;            //todo add to client  SO_SNDLOWAT
    ::setsockopt(client_fd, SOL_SOCKET, SO_RCVLOWAT, (char *)&recvLowWaterNum, sizeof(recvLowWaterNum));
    int sndLowWaterNum = kTcpWriteBuffSize;
    ::setsockopt(client_fd, SOL_SOCKET, SO_SNDLOWAT, (char *)&sndLowWaterNum, sizeof(sndLowWaterNum));
    
    int on = 1;
    ::setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(on));
    ::setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on));
    
    EvLoop::Ptr loop = nullptr;
    
    if(m_isSingleThread){
        loop = m_mainLoop;
    }else{
        loop = this->m_loops->GetAvailLoop();
    }
    
    if(loop.get() == nullptr){
        ErrorLog <<"get avail loop for conn failed "<<GetPrintName();
        exit(EXIT_FAILURE);
    }
    
    Channel::Ptr chan = std::make_shared<Channel>(client_fd, loop);
    
    Connection::Ptr con = GetNewConnection(loop, chan, hostBuf, port);
    
    con->SetCloseCallback(std::bind(&net::TcpServer::tcpCloseCallback, this, std::placeholders::_1));
    con->SetReadCallback(m_readCallback);
    
    this->m_connections.set(client_fd, con);
    
    chan->SetConnection(con);
    chan->SetReadCallback(std::bind(&Connection::PollReadCallback, con, chan));
    chan->SetWriteCallback(std::bind(&Connection::PollWriteCallback, con, chan));
    chan->EnableRead();
    chan->UpdateEvent();
    
    loop->AddChannel(chan);
    
    InfoLog <<"tcp up: "<<con->GetRemoteAddr() << " :" << con->GetRemotePort() <<" fd:"<<chan->GetFd();
    
    if(this->m_acceptCallback){
        m_acceptCallback(con);
    }
    
    return 0;
}




int net::TcpServer::tcpCloseCallback(std::shared_ptr<Connection> con){
    
    if(m_closeCallback){
        m_closeCallback(con);
    }
    
    m_connections.erase(con->GetFd());
    InfoLog <<"tcp down: "<<con->GetRemoteAddr() << " :" << con->GetRemotePort();
    
    return 0;
}























