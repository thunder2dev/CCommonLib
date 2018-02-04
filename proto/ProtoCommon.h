//
//  ProtoCommon.h
//  marstcp
//
//  Created by meryn on 2017/09/27.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef ProtoCommon_h
#define ProtoCommon_h

namespace google {
    namespace protobuf{
        class Message;        
    }
}


namespace net {
    
    const int kMsgIdHeadBits = 4;
    const int kMsgSeqBits = 4;
    const int kMsgCatBits = 4;    
    
    enum MsgCat: uint32_t{
        MsgCat_Req = 1,
        MsgCat_Rsp = 2,
        MsgCat_Chunk = 3
    };
    
    class Buff;
    class Connection;
    class Tcp;
    
    typedef int ProtoResponseCallback(std::shared_ptr<Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg);
    
    
}




#endif /* ProtoCommon_h */
