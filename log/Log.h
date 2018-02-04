//
//  Log.hpp
//  marstcp
//
//  Created by meryn on 2017/08/02.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Log_hpp
#define Log_hpp

#include <string>
#include <memory>
#include <iostream>
#include <thread>
#include "Utils.h"

#include "ConsoleLog.h"
#include "FileLog.h"

//#define MY_RELEASE

enum LogLevel{
    Debug,
    Warning,
    Info,
    Error,
    Fatal,
};

std::string getLevelString(LogLevel lv);



template<class T>
class Log{
    
public:
    Log(std::string name);
    
    std::ostream& GetPreparedStream(LogLevel lv);
    std::ostream& GetStream();
    
    std::mutex& GetMutex();
    
public:
    std::shared_ptr<T> logImpl;
    
};


template<class T> std::mutex& Log<T>::GetMutex(){
    return this->logImpl->GetMutex();
}

template<class T> std::ostream& Log<T>::GetPreparedStream(LogLevel lv){
    
    if(lv == LogLevel::Error){
        
        if(errno != 0){
            
            return this->logImpl->GetStream() <<  common::getLogTimestamp() << '|' << getLevelString(lv) << '|'  << std::this_thread::get_id() << '|' ;
            
        }
        
    }
    
    return this->logImpl->GetStream() <<  common::getLogTimestamp() << '|' << getLevelString(lv) << '|'  << std::this_thread::get_id() << '|' ;
}

template<class T> Log<T>::Log(std::string name):logImpl(std::make_shared<T>(name)){
}

template<class T> std::ostream& Log<T>::GetStream(){
    return this->logImpl->GetStream();
}


template<class T>
class PrintLine{
public:
    PrintLine(Log<T>& logger, LogLevel lv);
    ~PrintLine(){
        logger.GetStream() << std::endl;
    }
    std::ostream& GetStream();
    
private:
    std::lock_guard<std::mutex> lock;
    LogLevel lv;
    Log<T>& logger;
};

template<class T> PrintLine<T>::PrintLine(Log<T>& _logger, LogLevel _lv):lock(_logger.GetMutex()), logger(_logger), lv(_lv){
}


template<class T> std::ostream& PrintLine<T>::GetStream(){
    
    return this->logger.GetPreparedStream(lv);
    
}

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef MY_RELEASE
typedef FileLog LogDev;
#define PrintLog(logObj, level) ( PrintLine<FileLog>(logObj, level) \
                                .GetStream() \
                                << __FILENAME__ << '[' << __LINE__ << "](" << __func__ << ")| " )

#else
typedef ConsoleLog LogDev;
#define PrintLog(logObj, level) ( PrintLine<ConsoleLog>(logObj, level) \
                                .GetStream() \
                                << __FILENAME__ << '[' << __LINE__ << "](" << __func__ << ")| " )

#endif

#define NewLogType(typeName) (    )



extern Log<LogDev> netlog;
#define DebugLog PrintLog(netlog, LogLevel::Debug)
#define InfoLog  PrintLog(netlog, LogLevel::Info)
#define ErrorLog PrintLog(netlog, LogLevel::Error)
#define FatalLog PrintLog(netlog, LogLevel::Error)





#endif /* Log_hpp */















