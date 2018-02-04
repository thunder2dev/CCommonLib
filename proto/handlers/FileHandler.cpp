//
//  FileHandler.cpp
//  marstcp
//
//  Created by meryn on 2017/10/11.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "FileHandler.h"
#include "net/Tcp.h"
#include "net/Connection.h"
#include "hall-server/HallServer.h"
#include "proto-msg/pcommon/common.pb.h"
#include "proto/state-machines/FileStateMachine.h"

int FileDownloadHandler::handle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg){
    std::shared_ptr<net::ProtoServer> svr = std::static_pointer_cast<mars::HallServer>(_tcp);
    std::shared_ptr<common::FileRequest> req = std::static_pointer_cast<common::FileRequest>(_msg);
    
    const std::string& filePath = req->path();
    std::string fullPath = common::GetResourceDir() + "/" + filePath;
    
    InfoLog << "file req server:"<< _tcp->GetPrintName() << " msg ok:"<< (_msg->IsInitialized()) <<" req:"<< filePath;
    
    std::shared_ptr<common::FileResponse> rsp = std::make_shared<common::FileResponse>();  
    
    rsp->set_fileseq(-1);
    rsp->set_filesize(-1);
    
    do{
        if(!common::isRegularFile(fullPath)){
            ErrorLog <<"file not exists:"<<fullPath;
            rsp->set_status(common::FileNotExists);
            break;
        }
        
        int64_t fileSize = common::GetFileSizeByPath(fullPath);
        if(fileSize < 0){
            ErrorLog << "file size failed, " << fullPath << " "<< strerror(static_cast<int>(fileSize));
            rsp->set_status(common::FileError);
            break;
        }
        
        if (fileSize > common::FILE_MAX_SIZE){
            ErrorLog << "file size too big, " << fullPath << " " <<fileSize;
            rsp->set_status(common::FileError);
            break;
            
        }
        
        int64_t fileSeq = svr->StartSendFile(_conn, svr, filePath);
        if (fileSeq< 0){
            rsp->set_status(common::FileError);
            break;
        }
        
        rsp->set_filesize(static_cast<uint32_t>(fileSize));
        rsp->set_fileseq(static_cast<uint32_t>(fileSeq));
        rsp->set_status(common::FileOK);
        
    }while(0);
    
    svr->SendProtoResponse(_conn, shared_from_this(), rsp);
    
    return 0;
}



int FileAckHandler::handle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg){
    
    using namespace net::fileTrigger;
    
    std::shared_ptr<net::ProtoServer> svr = std::static_pointer_cast<mars::HallServer>(_tcp);
    std::shared_ptr<common::FileAck> req = std::static_pointer_cast<common::FileAck>(_msg);
    
    int64_t fileSeq = req->fileseq();
    uint32_t readIdx = req->packageid();
    
    InfoLog <<"get ack for file:" << fileSeq << " idx:" <<readIdx;
    
    std::shared_ptr<net::FileStateMachine> fsm = svr->GetFileStateMachine(fileSeq);
    
    if(fsm.get() == nullptr){
        ErrorLog <<"fsm missing for fileseq" << fileSeq;
        _conn->Close();
        return -1;
    }
    
    fsm->Fire(ackTrigger, fsm, readIdx);
    
    return 0;
}


int FileErrorHandler::handle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg){
    
    std::shared_ptr<net::ProtoServer> svr = std::static_pointer_cast<mars::HallServer>(_tcp);
    std::shared_ptr<common::FileEnd> req = std::static_pointer_cast<common::FileEnd>(_msg);
    
    uint32_t status = req->status();
    int64_t fileSeq = req->fileseq();
    
    std::shared_ptr<net::FileStateMachine> fsm = svr->GetFileStateMachine(m_reqSeq);

    if(fsm.get() == nullptr){
        ErrorLog <<"fsm missing for fileseq" << fileSeq;
        _conn->Close();
        return -1;
    }
    
    InfoLog <<"file end received :"<< fsm->GetFileName() <<" status:"<< status;

    return fsm->Finish(status);
}









































