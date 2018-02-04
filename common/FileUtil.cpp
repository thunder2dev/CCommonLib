//
//  FileUtil.cpp
//  marstcp
//
//  Created by meryn on 2017/09/07.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "FileUtil.h"
#include "log/Log.h"
#include <errno.h>
#include <fcntl.h>



int common::setNonBlocking(int _fd){
    
    int socketFlag = fcntl(_fd, F_GETFL, 0);
    socketFlag |= O_NONBLOCK;
    if(fcntl(_fd, F_SETFL, socketFlag) < 0){
        ErrorLog << " socket fd cannot nonblock " << strerror(errno);
        return -1;
    }
    
    return 0;
}

int common::setClosExec(int _fd){
    
    int socketFlag = fcntl(_fd, F_GETFL, 0);
    socketFlag |= O_CLOEXEC;
    if(fcntl(_fd, F_SETFL, socketFlag) < 0){
        FatalLog << "socket fd cannot cloexec " << strerror(errno);
        return -1;
    }
    
    return 0;
    
}


