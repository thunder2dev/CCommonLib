//
//  ProtoDispatcherModule.cpp
//  marstcp
//
//  Created by meryn on 2017/10/10.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "ProtoDispatcherModule.h"
#include "ProtobufDispatcher.h"
#include "net/Connection.h"
#include "ProtoHandlerReg.h"


net::ProtoDispatcherModule::ProtoDispatcherModule(std::shared_ptr<ProtoConfig> _config)
:m_dispatcher(std::make_unique<ProtobufDispatcher>(_config)){
    RegAllHandlers(m_dispatcher);
}


int net::ProtoDispatcherModule::SendProtoRequest(std::shared_ptr<Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg){
    
    return this->m_dispatcher->SendProto(_conn, _msg, MsgCat_Req, _conn->GetSendSeq());
}

int net::ProtoDispatcherModule::SendProtoRequestCallback(std::shared_ptr<Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg, std::function<ProtoResponseCallback> _cb){
    
    return this->m_dispatcher->SendProtoCallback(_conn, _msg, MsgCat_Req, _conn->GetSendSeq(), _cb);
    
}


int net::ProtoDispatcherModule::SendProtoResponse(std::shared_ptr<Connection> _conn, std::shared_ptr<ProtobufHandlerBase> _handler, std::shared_ptr<google::protobuf::Message> _msg){
    
    return this->m_dispatcher->SendProto(_conn, _msg, MsgCat_Rsp, _handler->GetReqSeq());
    
}

int64_t net::ProtoDispatcherModule::StartSendFile(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _srv, std::string _filePath){
    
    return this->m_dispatcher->StartSendFile(_conn, _srv, _filePath);
    
}


int net::ProtoDispatcherModule::StartReceiveFile(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _srv, std::string _fullPath, int64_t _fileSeq, uint32_t _fileSize, common::CommonRetCallback _cb){
    
    return this->m_dispatcher->StartReceiveFile(_conn, _srv, _fullPath, _fileSeq, _fileSize, _cb);
    
}

int net::ProtoDispatcherModule::SendChunk(std::shared_ptr<net::Connection> _conn, char* _bytes, int _size, int64_t _fileSeq, uint32_t _packIdx){
    
    return this->m_dispatcher->SendChunck(_conn, _bytes, _size, _fileSeq, _packIdx);
    
}

std::shared_ptr<net::FileStateMachine> net::ProtoDispatcherModule::GetFileStateMachine(int64_t _fileSeq){
    
    return m_dispatcher->GetFileStateMachine(_fileSeq);
    
}





























