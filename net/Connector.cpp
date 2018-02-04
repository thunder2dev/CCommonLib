//
//  Connector.cpp
//  marstcp
//
//  Created by meryn on 2017/09/12.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "Connector.h"
#include "common/TimerEvent.h"
#include "common/FileUtil.h"
#include "EvLoop.h"
#include "Channel.h"
#include "log/Log.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int startTryIntervalMilli = 500;
const int maxTryIntervalMilli = 1000 * 3600;


net::Connector::Connector(std::shared_ptr<EvLoop> _loop, std::string _ip, int _port, std::string _name /*=""*/)
:net::Connection(_loop, nullptr, _ip, _port),
m_name(_name)
{
}


void net::Connector::Connect(){
    
    m_loop->SetInterval(0, startTryIntervalMilli, std::bind(&net::Connector::ConnectOnceTry, std::static_pointer_cast<net::Connector>(shared_from_this()) ,std::placeholders::_1));
    
}

int net::Connector::ConnectOnceTry(std::shared_ptr<common::TimerEvent> ev){
    
    do{
        
        InfoLog <<"Connector try once";
        
        int fd;
        
        struct sockaddr_in remote_addr;
        memset(&remote_addr,0,sizeof(remote_addr));
        remote_addr.sin_family=AF_INET;
        remote_addr.sin_addr.s_addr=inet_addr(m_ip.c_str());
        remote_addr.sin_port=htons(m_port);
        
        if((fd = socket(PF_INET,SOCK_STREAM,0))<0)
        {
            ErrorLog << "new socket failed: " << this->GetDescription() << strerror(errno);
            break;
        }
        
        if(connect(fd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)
        {
            ErrorLog << "connect failed: " << this->GetDescription() << strerror(errno);
            break;
        }
        
        if(common::setNonBlocking(fd) < 0){
            ErrorLog <<"client nonblocking failed:"<<m_port;
            exit(EXIT_FAILURE);
        }
        
        if(common::setClosExec(fd) < 0){
            ErrorLog <<"client closexec failed:"<<m_port;
            exit(EXIT_FAILURE);
        }
        
        Channel::Ptr chan = std::make_shared<Channel>(fd, m_loop);
        chan->SetConnection(shared_from_this());
        chan->SetReadCallback(std::bind(&Connection::PollReadCallback, std::static_pointer_cast<net::Connector>(shared_from_this()), chan));
        chan->SetWriteCallback(std::bind(&Connection::PollWriteCallback, std::static_pointer_cast<net::Connector>(shared_from_this()), chan));
        chan->EnableRead();
        chan->UpdateEvent();
        
        this->m_chan = chan;
        
        m_loop->AddChannel(chan);
        
        if(m_connectedCallback){
            
            m_connectedCallback(shared_from_this());
            
        }
        
        
        ev->Stop();
        
        
        return 0;
        
    }while(0);
    
    
    auto inteval = ev->GetInterval();
    auto newInteval = inteval * 2;
    if(newInteval.count() < maxTryIntervalMilli){
        ev->SetInterval(newInteval);
    }
    
    return -1;
}


std::ostream& operator<<(std::ostream& os, const net::Connector& con)
{
    os << con.GetDescription();
    return os;
}































