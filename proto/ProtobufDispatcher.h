//
//  DispatcherImpl.h
//  marstcp
//
//  Created by meryn on 2017/09/19.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef DispatcherImpl_h
#define DispatcherImpl_h

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <memory>
#include <string>
#include <map>

#include "common/common.h"
#include "common/callbacks.h"

#include "ProtoCommon.h"
#include "ProtobufHandlerBase.h"

#define GetClassName(T) #T


namespace net {
    
    class FileStateMachine;
    class ProtoDispatcherModule;
    
    class HandlerUnit{
    public:
        HandlerUnit(int _msgId, std::string _msgClassName, std::shared_ptr<ProtobufHandlerBase> _handler, const google::protobuf::Message* _msgPrototype, bool _hasRsp)
        :m_msgId(_msgId), m_msgClassName(_msgClassName), m_handler(_handler), m_msgPrototype(_msgPrototype), m_hasRsp(_hasRsp)
        {
        }
        
    public:
        const google::protobuf::Message* m_msgPrototype;
        std::shared_ptr<ProtobufHandlerBase> m_handler;
        int m_msgId;
        std::string m_msgClassName;
        bool m_hasRsp;
        
    };
    
    class FileChunkInfo{
    public:
        int64_t m_fileSeq;
        uint32_t m_packIdx;
        uint32_t m_size;
        
    };
    
    
    
    
    class Buff;
    class Connection;
    class ProtoServer;
    class ProtoClient;
    class ProtoConfig;
    
    class ProtobufDispatcher{
    
    public:
        ProtobufDispatcher(std::shared_ptr<ProtoConfig> _config):m_config(_config)
        {
        }
        
        ~ProtobufDispatcher(){
        }
        
        int ConnectionReadCallback(std::shared_ptr<Buff> _buf);
        
        template<class H, class M>
        int RegisterHandlerNoResponse(int _msgId){
            
            const google::protobuf::Descriptor* descriptor = M::descriptor();
            assert(descriptor != nullptr);
            const google::protobuf::Message* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
            assert(prototype != nullptr);
            
            std::shared_ptr<H> handler = std::make_shared<H>();
            
            std::shared_ptr<HandlerUnit> unit = std::make_shared<HandlerUnit>(_msgId, descriptor->full_name(), std::static_pointer_cast<ProtobufHandlerBase>(handler), prototype, false);
            
            m_handlers[_msgId] = unit;
            m_protoName2IdDic[descriptor->full_name()] = _msgId;
            
            return 0;
            
        }
        
        template<class H, class M, class R>
        int RegisterHandlerWithResponse(int _msgId, int _rmsgId){
            
            const google::protobuf::Descriptor* descriptor1 = M::descriptor();
            assert(descriptor1 != nullptr);
            const google::protobuf::Message* prototype1 =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor1);
            assert(prototype1 != nullptr);
            
            std::shared_ptr<H> handler = std::make_shared<H>();
            
            std::shared_ptr<HandlerUnit> unit1 = std::make_shared<HandlerUnit>(_msgId, descriptor1->full_name(), std::static_pointer_cast<ProtobufHandlerBase>(handler), prototype1, true);
            
            m_handlers[_msgId] = unit1;
            m_protoName2IdDic[descriptor1->full_name()] = _msgId;
            
            const google::protobuf::Descriptor* descriptor2 = R::descriptor();
            assert(descriptor2 != nullptr);
            const google::protobuf::Message* prototype2 =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor2);
            assert(prototype2 != nullptr);
            
            std::shared_ptr<HandlerUnit> unit2 = std::make_shared<HandlerUnit>(_rmsgId, descriptor2->full_name(), nullptr, prototype2, true);
            
            m_handlers[_rmsgId] = unit2;            
            m_protoName2IdDic[descriptor2->full_name()] = _rmsgId;
            
            
            return 0;
            
        }
        
        template<class M>
        int RegisterSyncHandler(int _msgId){
            const google::protobuf::Descriptor* descriptor = M::descriptor();
            assert(descriptor != nullptr);
            const google::protobuf::Message* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
            assert(prototype != nullptr);
            
            std::shared_ptr<ProtobufHandlerNull> handler = std::make_shared<ProtobufHandlerNull>();
            
            std::shared_ptr<HandlerUnit> unit = std::make_shared<HandlerUnit>(_msgId, descriptor->full_name(), std::static_pointer_cast<ProtobufHandlerBase>(handler), prototype, false);
            
            m_handlers[_msgId] = unit;
            m_protoName2IdDic[descriptor->full_name()] = _msgId;
            
            return 0;
        }
        
        
        int SendProtoCallback(std::shared_ptr<Connection>, std::shared_ptr<google::protobuf::Message> msg, MsgCat _cat, int64_t seq, std::function<ProtoResponseCallback> _cb);
        int SendProto(std::shared_ptr<Connection>, std::shared_ptr<google::protobuf::Message> msg, MsgCat _cat, int64_t seq);
        int SendChunck(std::shared_ptr<net::Connection> _conn, char* _bytes, int _size, int64_t _fileSeq, uint32_t _packIdx);
        
        int64_t StartSendFile(std::shared_ptr<Connection> _conn,std::shared_ptr<Tcp> _svr, std::string _filePath);
        int StartReceiveFile(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _srv, std::string _fullPath, int64_t _fileSeq, uint32_t _fileSize, common::CommonRetCallback _cb);
        
        int GetMsgIdFromMsgName(std::string _msgName);
        std::string GetMsgNameFromMsgId(int _msgId);
        
        std::shared_ptr<FileStateMachine> GetFileStateMachine(int64_t _fileSeq){
            if(m_fileStateMachines.count(_fileSeq) <= 0){
                return nullptr;
            }
            return m_fileStateMachines[_fileSeq];
        }
        
    private:
        
        int encodeProto(std::shared_ptr<Buff> _buff, std::shared_ptr<google::protobuf::Message> _msg, MsgCat _cat, int64_t seq);
        int decodeProtoAndHandle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<Buff> _buff);
        
        int encodeChunk(std::shared_ptr<net::Buff> _buff, const char* _data, int _length, int64_t _fileSeq, uint32_t _packIdx);
        int decodeChunkAndHandle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<Buff> _buff);
        
        std::shared_ptr<HandlerUnit> getHandlerUnitFromMsgId(int _msgId);
        std::shared_ptr<HandlerUnit> getHandlerUnitFromMsg(std::shared_ptr<google::protobuf::Message> _msg);
        
    private:
        std::shared_ptr<ProtoConfig> m_config;
        
        std::map<int, std::shared_ptr<HandlerUnit>> m_handlers;
        std::map<std::string, int> m_protoName2IdDic;
        
        std::map<int64_t, std::shared_ptr<FileStateMachine>> m_fileStateMachines;
        
        //char m_chunkDecodeBuff[common::FILE_TRANSFER_PACK_SIZE];
        
        friend ProtoServer;
        friend ProtoClient;
    };

    
    
    
}

#endif /* DispatcherImpl_h */
















