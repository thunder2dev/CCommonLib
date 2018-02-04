//
//  HandlerReg.cpp
//  marstcp
//
//  Created by meryn on 2017/09/25.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "ProtoHandlerReg.h"
#include "proto/ProtobufDispatcher.h"
#include "proto-msg/pcommon/common.pb.h"
#include "proto/handlers/FileHandler.h"


void net::RegAllHandlers(std::shared_ptr<net::ProtobufDispatcher> _dispatch){
    
    _dispatch->RegisterHandlerWithResponse<FileDownloadHandler, common::FileRequest, common::FileResponse>(common::MsgId_FileReq, common::MsgId_FileRsp);
    _dispatch->RegisterHandlerNoResponse<FileErrorHandler,  common::FileEnd>(common::MsgId_FileEnd);
    _dispatch->RegisterHandlerNoResponse<FileAckHandler, common::FileAck>(common::MsgId_FileAck);
    
}












