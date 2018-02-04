//
//  FileHandler.hpp
//  marstcp
//
//  Created by meryn on 2017/10/11.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef FileHandler_hpp
#define FileHandler_hpp


#include "proto/ProtobufHandlerBase.h"

namespace net {
    class Tcp;
    class Connection;
}


class FileDownloadHandler: public net::ProtobufHandler<FileDownloadHandler>{
    int handle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg);
};

class FileAckHandler: public net::ProtobufHandler<FileAckHandler>{
    int handle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg);
};

class FileErrorHandler: public net::ProtobufHandler<FileErrorHandler>{
    int handle(std::shared_ptr<net::Tcp> _tcp, std::shared_ptr<net::Connection> _conn, std::shared_ptr<google::protobuf::Message> _msg);
};



#endif /* FileHandler_hpp */
