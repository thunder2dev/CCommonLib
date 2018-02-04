//
//  Poll.cpp
//  marstcp
//
//  Created by meryn on 2017/08/02.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "KQueuePoll.h"
#include <sys/socket.h>
#include "log/Log.h"
#include "Channel.h"




net::KQueuePoll::KQueuePoll(){
    
    
    this->pollFd = kqueue();
    
}


int net::KQueuePoll::AddChannel(std::shared_ptr<Channel>& chan){
    
    if(this->channels.find(chan->GetFd()) != this->channels.end()){
        ErrorLog << "fd is already in poll" << chan->GetFd();
        return -1;
    }
    
    this->UpdateEvent(chan);
    
    this->channels[chan->GetFd()] = chan;
    
    return 0;
}


int net::KQueuePoll::UpdateEvent(std::shared_ptr<Channel>& chan){

    bool is_modify = false;
    struct kevent ev[2];
    
    int n = 0;
    int fd = chan->GetFd();
    int event = chan->GetEvent();
    int old_event = chan->GetOldEvent();
    
    if(this->channels.find(fd) != this->channels.end()){
        is_modify = true;
    }

    if(event & kReadEvent){
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD|EV_ENABLE, 0, 0, (void*)(intptr_t)fd);
    }else if(is_modify && (old_event & kReadEvent)){
        EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, (void*)(intptr_t)fd);
    }
    
    if(event & kWriteEvent){
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD|EV_ENABLE, 0, 0, (void*)(intptr_t)fd);
    }else if(is_modify && (old_event & kWriteEvent)){
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void*)(intptr_t)fd);
    }
    
    chan->SetOldEvent(event);
    
    int ret = kevent(this->pollFd, ev, n, NULL, 0, NULL);
    
    if(ret < 0){
        FatalLog << std::strerror(errno) << "| update poll failed";
        return -1;
    }

    
    return 0;
}


int net::KQueuePoll::Poll(int waitms){
    
    struct timespec timeout;
    timeout.tv_sec = waitms / 1000;
    timeout.tv_nsec = (waitms % 1000) * 1e6;
    
    int n = kevent(this->pollFd, NULL, 0, this->ev, common::MAX_POLL_WAIT_EVENTS_NUM, &timeout);
    
    //InfoLog <<"has event num:"<<n;
    for(int ii = 0; ii < n; ii ++){
        
        int fd = (int)(intptr_t)this->ev[ii].udata;
        int events = this->ev[ii].filter;
        
        auto iter = this->channels.find(fd);
        
        if(iter == this->channels.end()){
            FatalLog << "chan is not exist";
            continue;
        }
        
        if(events == EVFILT_READ){
            iter->second->GetReadCallback()(iter->second);
        }else if (events == EVFILT_WRITE){
            iter->second->GetWriteCallback()(iter->second);
        }else{
            FatalLog << "unkown kqueue event, fd:"<<fd << " events:" <<events;
        }
    }
    
    return n;
}


int net::KQueuePoll::DelChannel(std::shared_ptr<Channel>& chan){
    
    chan->ResetEvent();
    UpdateEvent(chan);
    
    this->channels.erase(chan->GetFd());
    
    return 0;
    
}











































