//
//  ProtoDispatcherModule.hpp
//  marstcp
//
//  Created by meryn on 2017/10/10.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef ProtoDispatcherModule_hpp
#define ProtoDispatcherModule_hpp

#include <memory>
#include <functional>

#include "common/callbacks.h"
#include "ProtoCommon.h"

namespace google{
    namespace protobuf{
        class Message;
    }
}

namespace net {
    
    class Buff;
    class Connection;
    
    class ProtobufHandlerBase;
    class ProtobufDispatcher;
    class ProtoConfig;
    class FileStateMachine;
    
    class ProtoDispatcherModule{
    public:
        ProtoDispatcherModule(std::shared_ptr<ProtoConfig> _config);
        
        int SendProtoRequest(std::shared_ptr<Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg);
        int SendProtoRequestCallback(std::shared_ptr<Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg, std::function<ProtoResponseCallback> _cb);
        
        int SendProtoResponse(std::shared_ptr<Connection> _conn, std::shared_ptr<ProtobufHandlerBase> _handler, std::shared_ptr<google::protobuf::Message> _msg);
        
        int64_t StartSendFile(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _srv, std::string _filePath);
        int StartReceiveFile(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _srv, std::string _fullPath, int64_t _fileSeq, uint32_t _fileSize, common::CommonRetCallback _cb);
        
        int SendChunk(std::shared_ptr<net::Connection> _conn, char* _bytes, int _size, int64_t _fileSeq, uint32_t _packIdx);
        
        std::shared_ptr<FileStateMachine> GetFileStateMachine(int64_t _fileSeq);
        
    protected:

        
    protected:
        std::shared_ptr<ProtobufDispatcher> m_dispatcher;
        
    };
    
    
}

#endif /* ProtoDispatcherModule_hpp */
