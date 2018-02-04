//
//  FileLog.hpp
//  marstcp
//
//  Created by meryn on 2017/08/03.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef FileLog_hpp
#define FileLog_hpp

#include<string>
#include<fstream>
#include"LogDevice.h"

class FileLog: public LogDevice{
    
public:
    FileLog(std::string _name);
    
    void Print(int count, ...);
    std::mutex& GetMutex(){
        return mtx;
    }
    
    std::ostream& GetStream(){
        return out;
    }
    
private:
    std::string name;
    std::ofstream out;
    std::mutex mtx;
    
};

#endif /* FileLog_hpp */
