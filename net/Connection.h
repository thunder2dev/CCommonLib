//
//  Connection.hpp
//  marstcp
//
//  Created by meryn on 2017/08/16.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Connection_hpp
#define Connection_hpp

#include<string>
#include<functional>
#include"common/callbacks.h"
#include"common/Noncopyable.h"
#include"Buff.h"
#include"ConnectionExt.h"
#include "observers.h"


namespace net {
    
const int kMaxReadMsgOnce = 10;
const int kCountHeadBits = 4;

    
class Channel;
class EvLoop;
    
    
class Connection:public common::Noncopyable, public std::enable_shared_from_this<Connection>{   //Connection can only deleted from curr conn or after poll (in timeout process) to keep one Connection sync
public:
    typedef std::shared_ptr<Connection> Ptr;
    
    Connection(std::shared_ptr<EvLoop> _loop, std::shared_ptr<Channel> _chan, std::string _addr, int _port);
    
    int Send(std::shared_ptr<Buff> buff);
        
    int SendOutBuff();
    
    
    const std::string& GetRemoteAddr(){
        return m_ip;
    }
    
    const int& GetRemotePort(){
        return m_port;
    }
    
    std::string GetDescription() const{
        return "conn :" + m_ip + ":" + std::to_string(m_port);
    }
    
    std::shared_ptr<EvLoop> GetLoop(){ return m_loop; }
    std::shared_ptr<Channel> GetChannel(){ return m_chan; }
    
    void SetCloseCallback(common::ConnectionCloseCallback _closeCallback){ m_closeCallback = _closeCallback; }
    void SetReadCallback(std::function<common::TcpReadCallback> _readCallback){ m_readCallback = _readCallback; }
    void SetWriteCallback(std::function<common::PollWriteCallback> _writeCallback){ m_writeCallback = _writeCallback; }
    
    void AddOnCloseListener(std::shared_ptr<IConnectionCloseListener> _listener){ m_closeListeners.push_back(_listener); }
    void RemoveOnCloseListener(std::shared_ptr<IConnectionCloseListener> _listener){ m_closeListeners.erase(std::remove(m_closeListeners.begin(), m_closeListeners.end(), _listener), m_closeListeners.end()); }
    
    int64_t GetSendSeq(){ return ++m_reqseq; }
    
    void Close();
    bool IsClosed(){ return m_closed;}
    
    int GetFd();
    
    int PollReadCallback(std::shared_ptr<net::Channel> chan);
    int PollWriteCallback(std::shared_ptr<net::Channel> chan);
    
    std::shared_ptr<Buff> GetReadTmpBuff(){ return m_readTmpBuff; }
    std::shared_ptr<Buff> GetSendTmpBuff(){ return m_sendTmpBuff; };
    
    std::shared_ptr<ConnectionExt> GetExtModule(){ return m_ext; }
    void SetExtModule(std::shared_ptr<ConnectionExt> _ext) { m_ext = _ext; }
    
protected:
    
    int pollReadHead(std::shared_ptr<net::Channel> chan);
    int pollReadBody(std::shared_ptr<net::Channel> chan, const int32_t& bodyCount);
    
    void closeInLoop();
    void sendInLoop(std::shared_ptr<Buff> buff);
    

    
protected:
    const std::string m_ip;
    const int m_port;
    const std::shared_ptr<EvLoop> m_loop;
    std::shared_ptr<Channel> m_chan;
    
    bool m_closed;
    common::ConnectionCloseCallback m_closeCallback;
    std::function<common::TcpReadCallback> m_readCallback;
    std::function<common::PollWriteCallback> m_writeCallback;
    
    std::vector<std::shared_ptr<IConnectionCloseListener>> m_closeListeners;
    
    std::shared_ptr<Buff> m_readBuff;
    std::shared_ptr<Buff> m_sendBuff;
    
    std::shared_ptr<Buff> m_readTmpBuff;
    std::shared_ptr<Buff> m_sendTmpBuff;    //useless
    
    
    std::atomic<int64_t> m_reqseq;
    std::shared_ptr<ConnectionExt> m_ext;
    
    
};

}
    
#endif /* Connection_hpp */
