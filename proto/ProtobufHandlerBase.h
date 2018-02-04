//
//  ProtobufHandler.hpp
//  marstcp
//
//  Created by meryn on 2017/09/20.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef ProtobufHandler_hpp
#define ProtobufHandler_hpp


#include "common/Noncopyable.h"
#include "log/Log.h"
#include <google/protobuf/message.h>
#include <memory>



namespace net {

class Tcp;
class Connection;
    
    
class ProtobufHandlerBase: public common::Noncopyable, public std::enable_shared_from_this<ProtobufHandlerBase>{
public:
    virtual int handle(std::shared_ptr<Tcp> _tcp, std::shared_ptr<Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg){return 0;}
    virtual std::unique_ptr<ProtobufHandlerBase> clone() const{return nullptr;}
    
    void SetReqSeq(int64_t _seq){ m_reqSeq = _seq; }
    int64_t GetReqSeq(){ return m_reqSeq; }
    
    
protected:
    int64_t m_reqSeq;
};



template<class Derived>
class ProtobufHandler: public ProtobufHandlerBase{
public:
    virtual std::unique_ptr<ProtobufHandlerBase> clone() const{
        return std::make_unique<Derived>();
    }
    
    uint32_t GetReqSeq(){return m_reqSeq;}
    

};
    
    
class ProtobufHandlerNull: public ProtobufHandler<ProtobufHandlerNull>{
public:
    int handle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg){
        
        ErrorLog <<"null protobuf handler is called";
        
        return -1;
        
    }
    
    
};
    
    
}
    
    
    
#endif /* ProtobufHandler_hpp */

















