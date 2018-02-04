//
//  Channel.hpp
//  marstcp
//
//  Created by meryn on 2017/08/02.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Channel_hpp
#define Channel_hpp


#include <functional>
#include "common/callbacks.h"

namespace net {


const int kReadEvent = 1;
const int kWriteEvent = 2;

class EvLoop;
class Connection;

class Channel:public std::enable_shared_from_this<Channel>{
    
public:
    typedef std::shared_ptr<Channel> Ptr;
    
    Channel(int _fd, std::shared_ptr<EvLoop> loop):m_fd(_fd), m_loop(loop) ,m_event(0), m_oldEvent(0){
    }
    
    ~Channel();
    
    int GetFd(){return this->m_fd;}
    void SetFd(int _fd){this->m_fd = _fd;}
    
    
    void SetReadCallback(std::function<common::PollReadCallback> cbRead){ this->m_readCallback = cbRead; }
    std::function<common::PollReadCallback> GetReadCallback(){ return this->m_readCallback; }
    void SetWriteCallback(std::function<common::PollWriteCallback> cbRead){ this->m_writeCallback = cbRead; }
    std::function<common::PollWriteCallback> GetWriteCallback(){ return this->m_writeCallback; }
    
    void EnableRead(){ this->m_event |= kReadEvent;  }
    void EnableWrite(){ this->m_event |= kWriteEvent; }
    void DisableRead() { this->m_event &= (~kReadEvent); }
    void DisableWrite() {this->m_event &= (~kWriteEvent); }
    
    bool HasRead() { return (this->m_event & kReadEvent) > 0; }
    bool HasWrite() { return (this->m_event & kWriteEvent) > 0; }
    
    int GetEvent() { return m_event; }
    int GetOldEvent() { return m_oldEvent; }
    void SetOldEvent(int _old) { m_oldEvent = _old; }
    void ResetEvent() { m_event = 0; }
    int UpdateEvent();
    
    void SetConnection(std::shared_ptr<Connection> _conn){ m_conn = _conn; }
    std::weak_ptr<Connection> GetConnection() { return m_conn; }
    
private:
    
    int m_fd;
    int m_event;
    int m_oldEvent;
    std::function<common::PollReadCallback> m_readCallback;
    std::function<common::PollWriteCallback> m_writeCallback;
    std::weak_ptr<EvLoop> m_loop;
    
    std::weak_ptr<Connection> m_conn;
    
};

}

#endif /* Channel_hpp */
