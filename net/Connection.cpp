//
//  Connection.cpp
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include <errno.h>
#include <unistd.h>
#include <cassert>

#include "Connection.h"
#include "Channel.h"
#include "EvLoop.h"
#include "Buff.h"

net::Connection::Connection(std::shared_ptr<EvLoop> _loop, std::shared_ptr<Channel> _chan, std::string _addr, int _port)
:m_loop(_loop),
m_chan(_chan),
m_ip(std::move(_addr)),
m_port(_port),
m_closed(false),
m_readBuff(std::make_shared<Buff>()),
m_sendBuff(std::make_shared<Buff>(kTcpWriteBuffSize)),
m_readTmpBuff(std::make_shared<Buff>()),
m_sendTmpBuff(std::make_shared<Buff>(kTcpWriteBuffSize)),
m_ext(nullptr)
{
}

int net::Connection::GetFd(){
    return this->m_chan->GetFd();
}


void net::Connection::Close(){
    this->m_closed = true;
    this->m_loop->AddInLoopJob(std::bind(&net::Connection::closeInLoop, shared_from_this()));
}



void net::Connection::closeInLoop(){

    this->m_closeCallback(shared_from_this());
    m_loop->DelChannel(m_chan);
    
    
    
    
}

int net::Connection::Send(std::shared_ptr<Buff> buff){
    
    if(m_loop->IsInLoopThread()){
        sendInLoop(buff);
    }else{
        m_loop->AddInLoopJob(std::bind(&net::Connection::sendInLoop, shared_from_this(), buff));
    }
    
    return 0;
}




int net::Connection::SendOutBuff(){
    
    ssize_t count = write(m_chan->GetFd(), m_sendBuff->GetReadableCArr(), m_sendBuff->Length());
    if(count < 0){
        if(errno != EWOULDBLOCK && errno != EAGAIN){
            ErrorLog << "write error:"<< strerror(errno);
            Close();
            return -1;
        }
    }
    
    if(m_sendBuff->Length() <= 0){
        m_sendBuff->Reset();
        if(m_chan->HasWrite()){
            m_chan->DisableWrite();
            m_chan->UpdateEvent();
        }
    }else{
        
        if(!m_chan->HasWrite()){
            m_chan->EnableWrite();
            m_chan->UpdateEvent();
        }
        
    }
    
    return 0;
}





void net::Connection::sendInLoop(std::shared_ptr<Buff> buff){
    
    if(this->m_sendBuff->Length() <= 0){
        
        ssize_t count = write(m_chan->GetFd(), buff->GetReadableCArr(), buff->Length());
        InfoLog <<"write count:" << count << " conn:" << m_chan->GetFd();
        
        if(count < 0){
            if(errno != EWOULDBLOCK && errno != EAGAIN){
                ErrorLog << "write error:"<< strerror(errno);
                Close();
                return;
            }
        }
        
        buff->SetHaveConsumed(static_cast<int32_t>(count));
        
    }
    
    if(buff->Length() > 0){
        
        if(m_sendBuff->AvailCount() < buff->Length()){
            ErrorLog << "send avail buff is not enough:"<<m_sendBuff->AvailCount() << " expected:"<<buff->Length();
            Close();
            return;
        }
        
        assert(m_sendBuff->AppendBytes(buff->GetReadableCArr(), buff->Length()) == 0);
        buff->Reset();
        
        SendOutBuff();
    }else{
        
        m_chan->DisableWrite();
        m_chan->UpdateEvent();
    }
}



int net::Connection::pollReadHead(std::shared_ptr<net::Channel> chan){
    
    if(m_readBuff->Length() > kCountHeadBits || m_readBuff->Length() < 0){
        ErrorLog <<"read buff exceeds head count"<< m_readBuff->Length();
        return -1;
    }else if(m_readBuff->Length() == kCountHeadBits){
        return 0;
    }
    
    ssize_t count = read(chan->GetFd(), m_readBuff->GetWritableCArr(), kCountHeadBits - m_readBuff->Length());
    
    if(count <0 && ( errno == EAGAIN || errno == EWOULDBLOCK)){
        return 0;
    }else if(count <0){
        ErrorLog << "read err:" << std::strerror(errno);
        return -1;
    }
    
    if(count == 0){
        DebugLog <<"reading head count 0";
        return -1;
    }
    
    m_readBuff->SetHaveAppended(static_cast<int32_t>(count));
    
    /*
    int32_t test = 0;
    
     m_readBuff->PeekInt(test);
    
    InfoLog <<"peek :"<<test;*/
    
    return 0;
}


int net::Connection::pollReadBody(std::shared_ptr<net::Channel> chan, const int32_t& bodyCount){
    
    if(m_readBuff->Length() > bodyCount + kCountHeadBits || bodyCount > m_readBuff->AvailCount() || bodyCount < 0){
        ErrorLog<<"read buff exceeds bodycount:"<<bodyCount;
        return -1;
    }else if(bodyCount + kCountHeadBits == m_readBuff->Length()){
        return 0;
    }
    
    ssize_t count = read(chan->GetFd(), m_readBuff->GetWritableCArr(), bodyCount - (m_readBuff->Length() - kCountHeadBits));
    
    if(count <0 && ( errno == EAGAIN || errno == EWOULDBLOCK)){
        return 0;
    }else if(count <0){
        ErrorLog << "read err:" << std::strerror(errno);
        return -1;
    }
    
    if(count == 0){
        DebugLog <<"reading body count 0";
        return -1;
    }
    
    m_readBuff->SetHaveAppended(static_cast<int32_t>(count));
    
    return 0;
    
    
}



int net::Connection::PollReadCallback(std::shared_ptr<net::Channel> chan){
    
    std::shared_ptr<Connection> con = chan->GetConnection().lock();
    
    if(m_readBuff->AvailCount() <= 0){
        ErrorLog <<"read buff full" << con->GetDescription();
        con->Close();
        return 0;
    }
    
    int totReceived = 0;
    
    for(int ii = 0; ii < kMaxReadMsgOnce; ii++){
     
        if(pollReadHead(chan) < 0){
            con->Close();
            return 0;
        }

        if(m_readBuff->Length() < kCountHeadBits){
            return 0;
        }
        
        int32_t bodyCount = 0;
        if(m_readBuff->PeekInt(bodyCount) < 0){
            con->Close();
            return -1;
        }
        
        if(pollReadBody(chan, bodyCount) < 0){
            con->Close();
            return 0;
        }
        
        if(m_readBuff->Length() < bodyCount + kCountHeadBits){
            return 0;
        }
        
        InfoLog << con->GetDescription() <<" received :"<< m_readBuff->Length();
        
        totReceived += m_readBuff->Length();
        
        if(m_readCallback){
            m_readCallback(con, m_readBuff);
        }
        
        m_readBuff->Reset();
    }
    
    return 0;
}


int net::Connection::PollWriteCallback(std::shared_ptr<net::Channel> chan){
    
    InfoLog <<"write callback";
    
    std::shared_ptr<net::Connection> conn = chan->GetConnection().lock();
    
    conn->SendOutBuff();
    
    return 0;
}






















