//
//  FileLog.cpp
//  marstcp
//
//  Created by meryn on 2017/08/03.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "FileLog.h"

FileLog::FileLog(std::string _name)
:name(_name){
    out = std::ofstream("/Users/meryn/cpp/marstcp/" + _name + ".log");
}


void FileLog::Print(int count, ...){
    
    va_list ap;
    va_start(ap, count);
    for(int ii = 0; ii < count; ii++){
        this->out << va_arg(ap, char*);
    }
}
