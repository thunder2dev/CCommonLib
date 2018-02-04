//
//  ConsoleLog.cpp
//  marstcp
//
//  Created by meryn on 2017/08/03.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "ConsoleLog.h"

std::mutex ConsoleLog::mtx;
ConsoleLog::ConsoleLog(std::string _name)
:name(_name),out(std::cout){
}


void ConsoleLog::Print(int count, ...){
    
    va_list ap;
    va_start(ap, count);
    for(int ii = 0; ii < count; ii++){
        this->out << va_arg(ap, char*);
    }
}

