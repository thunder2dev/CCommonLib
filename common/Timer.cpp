//
//  Timer.cpp
//  marstcp
//
//  Created by meryn on 2017/09/07.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "Timer.h"
#include <sys/time.h>
#include "log/Log.h"
#include <unistd.h>
#include <fcntl.h>
#include "FileUtil.h"


/*
common::Timer::Timer(){
    
    int pipefd[2];
    if(pipe(pipefd) < 0){
        FatalLog <<"create time pipe failed err:" << strerror(errno);
        exit(EXIT_FAILURE);
    }
    
    m_readFd = pipefd[0];
    m_writeFd = pipefd[1];
    
    setClosExec(m_readFd);
    setClosExec(m_readFd);
    
    setNonBlocking(m_readFd);
}



void common::Timer::timerThread(){
    
    while(!this->m_stop){
        
        std::unique_lock<std::mutex> lck(this->m_cvmtx);
        
        if(m_cv.wait_for(lck, m_timeOutDuration.load()) == std::cv_status::no_timeout){
            continue;
        }
        
        write(m_writeFd, "t", 1);
        
    }
    
    
    close(m_readFd);
    close(m_writeFd);

}*/


//common::Timer* common::Timer::m_instance;


void common::timeAlarm(int signal){
    
    Timer::GetInstance()->AlarmHandler();
    
}


common::Timer::~Timer(){
    
    this->Stop();
    
    for(auto& fd : this->m_innerFds){
        close(fd);
    }
    
    for(auto& fd : this->m_outerFds){
        close(fd);
    }

}


void common::Timer::SetTimeout(std::chrono::microseconds micros, bool stop/* = false*/){
    std::lock_guard<std::mutex> lock_guard(m_mtx);
    
    struct itimerval iti;
    
    int64_t microCount = micros.count();
    
    iti.it_interval.tv_sec = 0;
    iti.it_interval.tv_usec = 0;
    
    if(stop){
        iti.it_value.tv_sec = 0;
        iti.it_value.tv_usec = 0;
    }else{
        iti.it_value.tv_sec = 0;
        iti.it_value.tv_usec = 1;
        
        if(micros.count() <= 0){
            iti.it_value.tv_sec = 0;
            iti.it_value.tv_usec = 1;
        }else{
            iti.it_value.tv_sec = static_cast<int>(microCount/1000000);
            iti.it_value.tv_usec = static_cast<int>(microCount%1000000);
        }
    }
    
    if(setitimer(ITIMER_REAL, &iti, NULL) < 0){
        ErrorLog << "set timer failed, err:" << std::strerror(errno);
    }
}





int common::Timer::GetNewListenFd(){
    
    std::lock_guard<std::mutex> lock_guard(m_mtx);
    
    int pipefd[2];
    if(pipe(pipefd) < 0){
        FatalLog <<"create time pipe failed err:" << strerror(errno);
        exit(EXIT_FAILURE);
    }
    
    int m_readFd = pipefd[0];
    int m_writeFd = pipefd[1];
    
    setClosExec(m_readFd);
    setClosExec(m_writeFd);
    
    setNonBlocking(m_writeFd);

    m_innerFds.push_back(m_writeFd);
    m_outerFds.push_back(m_readFd);
    
    return m_readFd;
}


void common::Timer::AlarmHandler(){
    
    //InfoLog <<"alarm !!!";
    
    std::lock_guard<std::mutex> lock_guard(m_mtx);
    
    
    for(auto& fd : m_innerFds){
        write(fd, "t", 1);
    }    
}


void common::Timer::Stop(){
    
    this->SetTimeout(std::chrono::microseconds(0), true);
    
    
}






































