//
//  FileStateMachine.cpp
//  marstcp
//
//  Created by meryn on 2017/10/11.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "FileStateMachine.h"

#include <cmath>

#include <unistd.h>

#include "fsm/State.h"
#include "fsm/StateConfig.h"
#include "common/TimerEvent.h"
#include "net/Connection.h"
#include "proto/ProtoServer.h"
#include "proto/ProtoClient.h"

#include "proto-msg/pcommon/common.pb.h"


//std::shared_ptr<fsm::TriggerWithParameter<int(net::FileStateMachine::*)(int32_t), &net::FileStateMachine::ackTriggerHandler>> net::fileTrigger::ackTrigger = 


int64_t net::FileStateMachine::m_fileSeqGenerator;

net::FileStateMachine::FileStateMachine(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _server, int32_t _initState, bool _isServerMode, uint32_t _id)
:fsm::StateMachine(_server, _initState, _id),
m_conn(_conn), m_server(_server), m_isServerMode(_isServerMode)
{
    reset();
    if(m_isServerMode){
        m_fileSeq=m_fileSeqGenerator++;
        m_id = m_fileSeq;
    }
    
    
}

int net::FileStateMachine::AckTriggerHandler(uint32_t _packid){
    
    if(m_ackIdx > _packid){
        ErrorLog << "ack pack id err, prev:" <<m_ackIdx <<" ack:"<<_packid;
        return Finish(-1);
    }
    
    m_ackIdx = _packid;
    
    return 0;
}

void net::FileStateMachine::OnConnectionClosed(){
    
    ErrorLog <<"connection is closed sm err";
    Finish(-1);
    
}

std::shared_ptr<net::FileStateMachine> net::NewServerFileStateMachine(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _server){
    
    //auto triggerAck = std::make_shared<fsm::TriggerWithParameter<int(FileStateMachine::*)(int32_t), &FileStateMachine::ackTriggerHandler>>(FileServerTrigger_ACK);
    
    std::shared_ptr<FileStateMachine> sm = std::make_shared<FileStateMachine>(_conn, _server, FileServerState_Ready, true, 0);
    
    sm->SetResetHandler(std::bind(&FileStateMachine::reset, sm));
    sm->SetOnConnectionCloseListener(sm);
    
    sm->Config(FileServerState_Ready, "file ready")
    ->Permit(FileServerState_Sending, fileTrigger::startSendTrigger);
    
    
    sm->Config(FileServerState_Sending, "file sending")
    ->OnEntry(std::bind(&net::FileStateMachine::startSendEntryHandler, sm))
    ->PermitReentry(fileTrigger::ackTrigger)
    ->Tick(std::bind(&FileStateMachine::sendTickHandler, sm, std::placeholders::_1));
    
    
    
    return sm;
}


std::shared_ptr<net::FileStateMachine> net::NewClientFileStateMachine(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _server, int64_t _fileSeq){
    
    
    std::shared_ptr<FileStateMachine> sm = std::make_shared<FileStateMachine>(_conn, _server, FileClientState_Ready, false, _fileSeq);
    
    sm->SetResetHandler(std::bind(&FileStateMachine::reset, sm));
    sm->SetOnConnectionCloseListener(sm);
    
    sm->Config(FileClientState_Ready, "file ready")
    ->Permit(FileClientState_Receiving, fileTrigger::startReceiveTrigger);
    
    sm->Config(FileClientState_Receiving, "file receiving")
    ->OnEntry(std::bind(&net::FileStateMachine::startReceiveEntryHandler, sm))
    ->PermitReentry(fileTrigger::receiveTrigger);   
    
    return sm;
}



int net::FileStateMachine::startSendEntryHandler(){
    
    int64_t fileSize = common::GetFileSize(m_fd);
    
    if(fileSize < 0 || fileSize > std::numeric_limits<uint32_t>::max()){
        ErrorLog <<"file size is err:"<<fileSize;
        Finish(-1);
        return 0;
    }
    
    m_fileSize = static_cast<uint32_t>(fileSize);
    
    m_packNum = static_cast<uint32_t>(ceil(1.0 * m_fileSize/common::FILE_TRANSFER_PACK_SIZE));
    
    if(m_packNum <= 0){
        Finish(-1);
        return 0;
    }
    
    m_currPackIdx = 0;
    
    return 0;
    
}


int net::FileStateMachine::startReceiveEntryHandler(){
    
    /*int64_t fileSize = common::GetFileSize(m_fd);
     
     if(fileSize < 0 || fileSize > std::numeric_limits<uint32_t>::max()){
     ErrorLog <<"file size is err:"<<fileSize;
     Finish(-1);
     return 0;
     }
     
     m_fileSize = static_cast<uint32_t>(fileSize);*/
    
    m_packNum = static_cast<uint32_t>(ceil(1.0 * m_fileSize/common::FILE_TRANSFER_PACK_SIZE));
    
    if(m_packNum <= 0){
        Finish(-1);
        return 0;
    }
    
    m_currPackIdx = 0;
    
    return 0;
    
}


int net::FileStateMachine::sendError(){
    
    if(m_conn.get() == nullptr || m_conn->IsClosed()){
        InfoLog <<"conn is not available";
        return 0;
    }
    
    std::shared_ptr<ProtoDispatcherModule> dispatcher = nullptr;
    
    if(m_isServerMode){
        
        dispatcher = std::static_pointer_cast<ProtoDispatcherModule>(std::static_pointer_cast<ProtoServer>(m_server));
    }else{
        
        dispatcher = std::static_pointer_cast<ProtoDispatcherModule>(std::static_pointer_cast<ProtoClient>(m_server));
    }
    
    
    auto fileEndMsg = std::make_shared<common::FileEnd>();
    
    fileEndMsg->set_status(common::FileError);
    
    return dispatcher->SendProtoRequest(m_conn, fileEndMsg);
    
}




int net::FileStateMachine::reset(){
    
    m_currPackIdx = 0;
    m_packNum = 0;
    m_ackIdx = 0;
    m_prevPackIdx = 0;
    m_handledCount = 0;
    
    return 0;
}

void net::FileStateMachine::finish(int _err){
    
    if(m_conn.get() != nullptr){
        m_conn->RemoveOnCloseListener(std::static_pointer_cast<net::FileStateMachine>(shared_from_this()));
    }
    
    if(_err != 0){
        sendError();
    }
    
    m_conn = nullptr;
    m_server = nullptr;
}


int net::FileStateMachine::sendTickHandler(std::shared_ptr<common::TimerEvent> _ev){
    
    if(!m_isRunning){
        _ev->Stop();
        return 0;
    }
    
    if(m_conn == nullptr || m_conn->IsClosed()){
        _ev->Stop();
        ErrorLog <<"m_conn is null or closed file"<<m_fileName;
        Finish(-1);
        return 0;
    }
    
    if( m_currPackIdx - m_ackIdx >= kMaxNoAckDiff){
        _ev->SetInterval(_ev->GetInterval() * 2);
        if(_ev->GetInterval().count() > common::FILE_TRANSFER_MAX_DURATION_MS){
            ErrorLog <<"ack is timeout" << " curr interval:"<<_ev->GetInterval().count()<< " max dur:"<<common::FILE_TRANSFER_MAX_DURATION_MS << " m_currPackIdx:" << m_currPackIdx <<" m_ackIdx:" << m_ackIdx;
            _ev->Stop();
            m_conn->Close();
            Finish(-1);
            return 0;
        }
        return 0;
    }
    
    
    int reqSize = common::FILE_TRANSFER_PACK_SIZE;
    if(m_currPackIdx + 1 == m_packNum){
        reqSize = m_fileSize - m_handledCount;
    }
    
    if(reqSize < 0){
        ErrorLog <<"require size less than 0";
        Finish(-1);
        return 0;
    }
    
    long n = read(m_fd, m_buff, reqSize);
    if(n < 0){
        ErrorLog <<"read file err, file:"<<m_fileName <<" err:"<<strerror(errno);
        _ev->Stop();
        Finish(-1);
        return 0;
    }
    
    if(n < common::FILE_TRANSFER_PACK_SIZE && n + m_handledCount != m_fileSize){
        ErrorLog <<"read file count err, file:"<<m_fileName <<" require:"<<common::FILE_TRANSFER_PACK_SIZE <<" indeed:"<<n;
        _ev->Stop();
        Finish(-1);
        return 0;
    }
    
    if(std::static_pointer_cast<ProtoServer>(m_server)->SendChunk(m_conn, m_buff, static_cast<int>(n), m_fileSeq , m_currPackIdx) != 0){
        
        ErrorLog <<" send chunk err, file:"<<m_fileName;
        _ev->Stop();
        Finish(-1);
        return 0;
        
    }
    
    m_prevPackIdx = m_currPackIdx;
    m_currPackIdx += 1;
    m_handledCount += n;
    
    if(m_currPackIdx >= m_packNum){
        InfoLog  << "file " << m_fileName <<" finished";
        _ev->Stop();
        return 0;
    }
    
    InfoLog <<"send file chunk :" << m_currPackIdx;
    
    return 0;
}


void net::FileStateMachine::SetOnConnectionCloseListener(std::shared_ptr<IConnectionCloseListener> _listener){
    
    m_conn->AddOnCloseListener(_listener);
}


int net::FileStateMachine::ReceiveTriggerHandler(uint32_t _packid, std::shared_ptr<Buff> _buff){
    
    static bool test = false;
    

    InfoLog <<"receving file packIdx:"<<_packid <<" fileSeq:"<<m_fileSeq;
    if(test){
        InfoLog <<"test here";
    }
    
    if(!m_isRunning){
        ErrorLog <<"fsm is dead, "<< m_fileName <<" seq:"<<m_fileSeq;
        return 0;
    }
    
    std::shared_ptr<ProtoClient> svr = std::static_pointer_cast<ProtoClient>(m_server);
    
    if(m_conn == nullptr || m_conn->IsClosed()){
        ErrorLog <<"m_conn is null or closed file"<<m_fileName;
        return Finish(-1);
    }
    
    long n = write(m_fd, _buff->GetReadableCArr(), _buff->Length());
    if(n < 0){
        ErrorLog <<"write file err, file:"<<m_fileName <<" err:"<<strerror(errno);
        return Finish(-1);
    }
    
    m_handledCount += n;
    
    if(m_currPackIdx+1 == m_packNum){
        if(m_handledCount == m_fileSize){
            return Finish(0);
        }else{
            ErrorLog <<"recieved size is not file size";
            return Finish(-1);
        }
    }
    
    if(m_currPackIdx % kAckFrequency == 0){
        
        auto ackmsg = std::make_shared<common::FileAck>();
        ackmsg->set_fileseq(m_fileSeq);
        ackmsg->set_packageid(m_currPackIdx);
        svr->SendProtoRequest(m_conn, ackmsg);
        InfoLog <<"send ack:"<<m_currPackIdx;
        
    }

    m_currPackIdx += 1;
    
    if(!test){
        test = true;
    }
    
    return 0;
}

int net::FileStateMachine::StartReceiveTriggerHandler(int _fd, int64_t _fileSeq, uint32_t _fileSize){
 
    m_fd = _fd;
    m_fileSeq = _fileSeq;
    m_fileSize = _fileSize;
    m_currPackIdx = m_prevPackIdx = m_handledCount = 0;
    m_packNum = static_cast<int>(ceil(m_fileSize/common::FILE_TRANSFER_PACK_SIZE));
    
    return 0;
    
}




int net::FileStateMachine::StartSendTriggerHandler(int _fd){
    
    m_fd = _fd;
    
    return 0;
    
}

















