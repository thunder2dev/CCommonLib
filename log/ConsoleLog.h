//
//  ConsoleLog.hpp
//  marstcp
//
//  Created by meryn on 2017/08/03.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef ConsoleLog_hpp
#define ConsoleLog_hpp

#include<string>
#include<iostream>
#include"LogDevice.h"

class ConsoleLog: public LogDevice{
    
public:
    ConsoleLog(std::string _name);
    
    void Print(int count, ...);
    std::mutex& GetMutex(){
        return ConsoleLog::mtx;
    }
    
    std::ostream& GetStream(){
        return out;
    }
    

    
    
private:
    std::string name;
    std::ostream &out;
    static std::mutex mtx;
    
};

#endif /* ConsoleLog_hpp */
