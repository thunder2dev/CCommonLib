//
//  ProtobufDispatcher.cpp
//  marstcp
//
//  Created by meryn on 2017/09/19.
//  Copyright © 2017 marsladder. All rights reserved.
//
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include "ProtobufDispatcher.h"
#include "net/Buff.h"
#include "net/Connection.h"
#include "ProtoConnectionExt.h"
#include "state-machines/FileStateMachine.h"


int net::ProtobufDispatcher::ConnectionReadCallback(std::shared_ptr<Buff> _buf){
    
    int32_t len = 0;
    int ret = _buf->ReadInt(len);
    
    if(ret != 0 || len != _buf->Length()){
        ErrorLog <<"read len failed";
        return -1;
    }
    
    int32_t msgId = 0;
    
    if(_buf->ReadInt(msgId) < 0){
        ErrorLog <<"read msg id failed";
        return -1;
    }
    
    return 0;
}


int net::ProtobufDispatcher::GetMsgIdFromMsgName(std::string _msgName){
    
    if(m_protoName2IdDic.count(_msgName) <= 0){
        return -1;
    }
    
    return m_protoName2IdDic[_msgName];
    
}




std::string net::ProtobufDispatcher::GetMsgNameFromMsgId(int _msgId){
    
    if(m_handlers.count(_msgId) <= 0){
        return "";
    }
    
    std::shared_ptr<HandlerUnit> unit = m_handlers[_msgId];
    
    return unit->m_msgClassName;
}


std::shared_ptr<net::HandlerUnit> net::ProtobufDispatcher::getHandlerUnitFromMsgId(int _msgId){
    
    if(m_handlers.count(_msgId) <= 0){
        return nullptr;
    }
    
    return m_handlers[_msgId];
}

std::shared_ptr<net::HandlerUnit> net::ProtobufDispatcher::getHandlerUnitFromMsg(std::shared_ptr<google::protobuf::Message> _msg){
    
    int msgId = GetMsgIdFromMsgName(_msg->GetTypeName());
    
    if(msgId <= 0){
        ErrorLog <<"msg not found by name:" <<_msg->GetTypeName();
        return nullptr;
    }
    
    return this->getHandlerUnitFromMsgId(msgId);
    
}


int net::ProtobufDispatcher::encodeChunk(std::shared_ptr<net::Buff> _buff, const char* _data, int _length, int64_t _fileSeq, uint32_t _packIdx){
    
    if(_buff->AppendBytes(_data, _length) != 0){
        ErrorLog <<"append bytes failed";
        return -1;
    }
    
    _buff->PrependInt(_packIdx);
    
    _buff->PrependInt64(_fileSeq);
    
    _buff->PrependUnit(MsgCat::MsgCat_Chunk);
    
    _buff->PrependInt(_buff->Length());
    
    return 0;
}

int net::ProtobufDispatcher::decodeChunkAndHandle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<Buff> _buff){
    
    using namespace google;
    
    std::shared_ptr<net::FileChunkInfo> info = std::make_shared<net::FileChunkInfo>();
    
    int64_t fileSeq = 0;
    
    if(_buff->ReadInt64(fileSeq) < 0){
        ErrorLog <<"read fileSeq from msg failed:";
        return -1;
    }
    
    if(m_fileStateMachines.count(fileSeq) <= 0){
        ErrorLog <<"file state machine not exists:"<<fileSeq;
        _conn->Close();
        return -1;
    }
    
    std::shared_ptr<FileStateMachine> fileStateMachine = m_fileStateMachines[fileSeq];
    
    uint32_t packIdx;
    
    if(_buff->ReadUint(packIdx) < 0){
        ErrorLog <<"read pack idx err:";
        return -1;
    }
    
    info->m_packIdx = packIdx;
    info->m_fileSeq = fileSeq;
    info->m_size = _buff->Length();
    
    
    fileStateMachine->Fire(fileTrigger::receiveTrigger, fileStateMachine, packIdx, _buff);
    
    
    
    
    
    return 0;
}





int net::ProtobufDispatcher::encodeProto(std::shared_ptr<net::Buff> _buff, std::shared_ptr<google::protobuf::Message> _msg, MsgCat _cat, int64_t seq){
    
    if(_msg->ByteSizeLong() > _buff->AvailCount()){
        ErrorLog << "msg length exceeds buff:" << _msg->GetTypeName() << " avail size:" << _buff->AvailCount();
        return -1;
    }
    
    if(!_msg->SerializeToArray(_buff->GetWritableCArr(), _buff->AvailCount())){
        ErrorLog << " msg serialize failed" << _msg->GetTypeName();
        return -1;
    }
    
    _buff->SetHaveAppended(static_cast<int32_t>(_msg->ByteSizeLong()));
    
    DebugLog <<"msg type:" <<_msg->GetTypeName() << " count:" <<m_protoName2IdDic.size() << " first:";
    
    int msgId = GetMsgIdFromMsgName(_msg->GetTypeName());
    
    if(msgId <= 0){
        ErrorLog <<"msg not found by name:" <<_msg->GetTypeName();
        return -1;   //todo return error msg or msgid and count will wrong
    }
    
    _buff->PrependInt64(seq);
    
    _buff->PrependInt(msgId);
    
    _buff->PrependUnit(_cat);
    
    _buff->PrependInt(_buff->Length());
    
    return 0;
}


int net::ProtobufDispatcher::decodeProtoAndHandle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<net::Buff> _buff){
    
    using namespace google;
    
    int32_t len = 0;
    if(_buff->ReadInt(len) < 0){
        ErrorLog <<"read len from msg failed:";
        return -1;
    }
    
    uint32_t cat = 0;
    
    if(_buff->ReadUint(cat) < 0){
        ErrorLog <<"read cat from msg failed:";
        return -1;
    }
    
    if(cat == MsgCat_Chunk){
        return decodeChunkAndHandle(_tcp, _conn, _buff);
    }
    
    int32_t msgId = 0;
    
    if(_buff->ReadInt(msgId) < 0){
        ErrorLog <<"read msg id from msg failed:";
        return -1;
    }
    
    std::shared_ptr<HandlerUnit> unit = getHandlerUnitFromMsgId(msgId);
    if(unit.get() == nullptr){
        ErrorLog <<"no msg with id" << msgId;
        return -1;
    }
    
    
    int64_t seq = 0;
    
    if(_buff->ReadInt64(seq) < 0){
        ErrorLog <<"read seq from msg failed:";
        return -1;
    }    
    
    std::shared_ptr<protobuf::Message> msg(unit->m_msgPrototype->New());
    
    if(_buff->Length() != len - kMsgIdHeadBits - kMsgSeqBits - kMsgCatBits){
        ErrorLog <<"buff len mismatched msg len";
        return -1;
    }
    
    msg->ParseFromArray(_buff->GetReadableCArr(), _buff->Length());
    
    _buff->Reset();
    
    
    std::shared_ptr<ProtoConnectionExt> ext = std::static_pointer_cast<ProtoConnectionExt>(_conn->GetExtModule());
    
    if(ext->HasRspCallback(seq) && cat == MsgCat_Rsp){
        std::function<ProtoResponseCallback> func = ext->PopRspHandler(seq);
        func(_tcp, _conn, msg);
    }else{
        
        std::shared_ptr<ProtobufHandlerBase> handler = unit->m_handler->clone();
        
        handler->SetReqSeq(seq);
        
        handler->handle(_tcp, _conn, msg);
    }
    
    return 0;
}


int net::ProtobufDispatcher::SendProtoCallback(std::shared_ptr<Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg, net::MsgCat _cat, int64_t _seq, std::function<ProtoResponseCallback> _cb){
    
    std::shared_ptr<ConnectionExt> _ext = _conn->GetExtModule();
    std::shared_ptr<ProtoConnectionExt> ext = std::static_pointer_cast<ProtoConnectionExt>(_ext);
    
    std::shared_ptr<HandlerUnit> unit = this->getHandlerUnitFromMsg(_msg);
    if(unit.get() == nullptr){
        ErrorLog << "get handler unit from msg failed:" << _msg->GetTypeName();
        return -1;
    }
    
    if(!unit->m_hasRsp){
        ErrorLog <<"this unit has not rsp callback:" << _msg->GetTypeName();
        return -1;
    }
    
    ext->AddRspCallback(_seq, _cb);
    
    SendProto(_conn, _msg, _cat, _seq);
    
    return 0;
}


int net::ProtobufDispatcher::SendProto(std::shared_ptr<net::Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg, MsgCat _cat, int64_t seq){
    
    std::shared_ptr<Buff> sendBuff = _conn->GetReadTmpBuff();
    sendBuff->Reset();
    
    this->encodeProto(sendBuff, _msg, _cat, seq);
    
    _conn->Send(sendBuff);
    
    return 0;
}

int net::ProtobufDispatcher::SendChunck(std::shared_ptr<net::Connection> _conn, char* _bytes, int _size, int64_t _fileSeq, uint32_t _packIdx){
    
    std::shared_ptr<Buff> sendBuff = std::make_shared<Buff>(kTcpChunkBuffSize);
    sendBuff->Reset();
    
    this->encodeChunk(sendBuff, _bytes, _size, _fileSeq, _packIdx);
    
    _conn->Send(sendBuff);
    
    return 0;
}





int64_t net::ProtobufDispatcher::StartSendFile(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _svr, std::string _filePath){
    
    std::string fullpath = common::GetResourceDir() + "/" + _filePath;
    
    int fd = open(fullpath.c_str(), O_RDONLY );
    if(fd <= 0){
        InfoLog <<"open file err:"<<strerror(errno);
        return -1;
    }
    
    std::shared_ptr<FileStateMachine> sm = NewServerFileStateMachine(_conn, _svr);
    
    if(m_fileStateMachines.count(sm->GetId()) > 0){
        
        ErrorLog <<"sm id is already taken :"<<sm->GetId();
        return -1;
        
    }
    
    m_fileStateMachines[sm->GetId()] = sm;
    
    InfoLog <<"new file state machine :" <<sm->GetId();
    
    sm->SetFinishHandler([this, &fd, _filePath](int err)-> int{
        if(err != 0){
            ErrorLog <<"file machine err, file:" << _filePath;
        }
        
        close(fd);
        
        m_fileStateMachines.erase(fd);
        
        return 0;
    });
    
    sm->Fire(fileTrigger::startSendTrigger, sm, fd);   
    
    
    return sm->GetId();
}




int net::ProtobufDispatcher::StartReceiveFile(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _svr, std::string _filefullPath, int64_t _fileSeq, uint32_t _fileSize, common::CommonRetCallback _cb){
    
    
    int fd = open(_filefullPath.c_str(), O_WRONLY | O_CREAT);
    if(fd <= 0){
        InfoLog <<"open file " << _filefullPath << " write err:"<<strerror(errno);
        return -1;
    }
    
    std::shared_ptr<FileStateMachine> sm = NewClientFileStateMachine(_conn, _svr, _fileSeq);
    
    if(m_fileStateMachines.count(sm->GetId()) > 0){
        
        ErrorLog <<"sm id is already taken :"<<sm->GetId();
        return -1;
        
    }
    
    m_fileStateMachines[sm->GetId()] = sm;
    
    InfoLog <<"new client file state machine, fileseq:"<<sm->GetId();
    
    sm->SetFinishHandler([this, &fd, &_filefullPath, _cb](int err)-> int{
        if(err != 0){
            ErrorLog <<"file machine err, file:" << _filefullPath;
        }
        
        close(fd);
        
        m_fileStateMachines.erase(fd);
        
        _cb(err);
        
        return 0;
    });
    
    sm->Fire(fileTrigger::startReceiveTrigger, sm, fd, _fileSeq, _fileSize);
    
    
    
    return 0;
}



































