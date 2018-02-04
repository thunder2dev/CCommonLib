//
//  FileStateMachine.hpp
//  marstcp
//
//  Created by meryn on 2017/10/11.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef FileStateMachine_hpp
#define FileStateMachine_hpp

#include "fsm/StateMachine.h"
#include "common/common.h"
#include "net/observers.h"

namespace common {
    class TimerEvent;
}
/*
namespace fsm {
 
}*/

namespace net {
    
    const int kMaxNoAckDiff = 7;
    const int kAckFrequency = 3;
    
    typedef std::function<int(int)> FileFinishCallback;
    
    class ProtoServer;
    class ProtoDispatcherModule;
    class Connection;
    class Buff;
    
    enum FileServerState:int32_t{
        FileServerState_Ready,
        FileServerState_Sending,
    };
    
    enum FileServerTrigger:int32_t{
        FileServerTrigger_Start,
        FileServerTrigger_ACK,
        FileServerTrigger_Connection_Lost,
    };
    
    
    enum FileClientState: int32_t{
        FileClientState_Ready,
        FileClientState_Receiving,
    };
    
    enum FileClientTrigger: int32_t{
        FileClientTrigger_Start,
        FileClientTrigger_NewPackage,
        FileClientTrigger_Error
    };

    
    
    class FileStateMachine: public fsm::StateMachine, public IConnectionCloseListener{
    public:
        FileStateMachine(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _server, int32_t _initState, bool _isServerMode, uint32_t _id);
        
        friend std::shared_ptr<FileStateMachine> NewServerFileStateMachine(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _server);
        friend std::shared_ptr<FileStateMachine> NewClientFileStateMachine(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp> _server, int64_t _fileSeq);
        
        void SetOnConnectionCloseListener(std::shared_ptr<IConnectionCloseListener> _listener);
    public:
        std::string GetFileName() { return m_fileName; }
        
        int StartSendTriggerHandler(int _fd);
        int AckTriggerHandler(uint32_t _packid);
        
        
        int StartReceiveTriggerHandler(int _fd, int64_t _fileSeq, uint32_t _fileSize);
        int ReceiveTriggerHandler(uint32_t _packid, std::shared_ptr<Buff> _buff);
        
    private:
        
        void OnConnectionClosed();
        
        int startSendEntryHandler();
        int startReceiveEntryHandler();
        int sendTickHandler(std::shared_ptr<common::TimerEvent> _ev);
        
        int sendError();
        
        int reset();
        void finish(int _err);
        
    private:
        
        static int64_t m_fileSeqGenerator;
        
        int64_t m_fileSeq;
        
        int m_fd;
        
        std::shared_ptr<Connection> m_conn;
        std::shared_ptr<Tcp> m_server;
        std::shared_ptr<ProtoDispatcherModule> m_dispatcher;
        
        uint32_t m_packNum;
        uint32_t m_currPackIdx;
        uint32_t m_prevPackIdx;
        uint32_t m_handledCount;
        
        uint32_t m_ackIdx;
        
        char m_buff[common::FILE_TRANSFER_PACK_SIZE];
        
        std::string m_fileName;
        uint32_t m_fileSize;
        bool m_isServerMode;
        
    };
    
    std::shared_ptr<FileStateMachine> NewServerFileStateMachine(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp>);
    std::shared_ptr<FileStateMachine> NewClientFileStateMachine(std::shared_ptr<Connection> _conn, std::shared_ptr<Tcp>, int64_t _fileSeq);
    
    namespace fileTrigger{
        
        const auto startSendTrigger = std::make_shared<fsm::TriggerWithParameter<int(net::FileStateMachine::*)(int), &net::FileStateMachine::StartSendTriggerHandler>>(FileServerTrigger_Start);
        const auto ackTrigger = std::make_shared<fsm::TriggerWithParameter<int(net::FileStateMachine::*)(uint32_t), &net::FileStateMachine::AckTriggerHandler>>(FileServerTrigger_ACK);
        
        
        
        const auto startReceiveTrigger = std::make_shared<fsm::TriggerWithParameter<int(net::FileStateMachine::*)(int, int64_t, uint32_t), &net::FileStateMachine::StartReceiveTriggerHandler>>(FileClientTrigger_Start);
        const auto receiveTrigger = std::make_shared<fsm::TriggerWithParameter<int(net::FileStateMachine::*)(uint32_t, std::shared_ptr<Buff>), &net::FileStateMachine::ReceiveTriggerHandler>>(FileClientTrigger_NewPackage);
        
    };
    
    
}




#endif /* FileStateMachine_hpp */
