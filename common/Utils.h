//
//  Utils.hpp
//  marstcp
//
//  Created by meryn on 2017/08/02.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace common {

    std::string timePoint2str(std::chrono::time_point<std::chrono::system_clock> pt);

    std::string getLogTimestamp();

    void printStackTrace();
    
    std::string getRealPath(std::string _relPath);
    
    bool isRegularFile(const std::string& path);
    
    bool isFileExists(const std::string& path);
    
    std::string GetResourceDir();
    std::string GetConfigDir();
    
    int64_t GetFileSize(int _fd);
    
    int64_t GetFileSizeByPath(std::string _path);

    
    std::string decrypt64coding(const std::string& str_in, const std::string& key, const std::string& iv);
    
    std::string encrypt64coding(const std::string& str_in, const std::string& key, const std::string& iv);
    
    
    int encrypt(const char* _input, const int& _inputLen, char* _output, int& _outputLen, const std::string& key, const std::string& iv);

    int decrypt(const char* _input, const int& _inputLen, char* _output, int& _outputLen, const std::string& key, const std::string& iv);
    
}

#endif /* Utils_hpp */







