//
//  Utils.cpp
//  marstcp
//
//  Created by meryn on 2017/08/02.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "Utils.h"
#include <execinfo.h>
#include <sys/stat.h>

#include "common.h"

#include "cryptopp/cryptlib.h"
#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "cryptopp/base64.h"
#include "log/Log.h"


std::string common::getLogTimestamp(){
    
    auto now = std::chrono::system_clock::now();
    return timePoint2str(now);
    
}

std::string common::timePoint2str(std::chrono::time_point<std::chrono::system_clock> pt){
    auto in_time_t = std::chrono::system_clock::to_time_t(pt);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(pt.time_since_epoch()) - std::chrono::duration_cast<std::chrono::seconds>(pt.time_since_epoch());
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S.");
    ss <<std::setw(3) << std::setfill('0') << ms.count();
    return ss.str();
}


void common::printStackTrace(){
    
    int size = 16;
    void * array[16];
    int stack_num = backtrace(array, size);
    char ** stacktrace = backtrace_symbols(array, stack_num);
    for (int i = 0; i < stack_num; ++i)
    {
        printf("%s\n", stacktrace[i]);
    }
    free(stacktrace);    
}


std::string common::getRealPath(std::string _relPath){
    
    char resolved_path[PATH_MAX];
    realpath(_relPath.c_str(), resolved_path);
    
    return resolved_path;
    
}


bool common::isRegularFile(const std::string& path)
{
    struct stat path_stat;
    
    if(stat(path.c_str(), &path_stat) != 0){
        return false;
    }
    return S_ISREG(path_stat.st_mode);
}


bool common::isFileExists(const std::string& path)
{
    struct stat path_stat;
    
    if(stat(path.c_str(), &path_stat) != 0){
        return false;
    }
    return true;
}

std::string common::GetResourceDir(){
    return ROOT_PATH + "/" + SUBDIR_RES;
}

std::string common::GetConfigDir(){
    return ROOT_PATH + "/" + SUBDIR_CONF;
}

int64_t common::GetFileSize(int _fd){
    
    struct stat st;
    if(fstat(_fd, &st) < 0){
        return -1;
    }
    
    return st.st_size;
    
}


int64_t common::GetFileSizeByPath(std::string _path){
    
    struct stat st;
    if(stat(_path.c_str(), &st) < 0){
        return -1;
    }
    
    return st.st_size;
    
}




int common::encrypt(const char* _input, const int& _inputLen, char* _output, int& _outputLen, const std::string& key, const std::string& iv){
    
    
    try {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption((byte*)key.c_str(), key.length(), (byte*)iv.c_str());
        
        
        CryptoPP::ArraySink cs((byte*)_output, _outputLen);
        
        CryptoPP::ArraySource encryptor((byte*)_input, _inputLen, true,
                                        new CryptoPP::StreamTransformationFilter(encryption,
                                                                                 new CryptoPP::Redirector(cs),
                                                                                 CryptoPP::AuthenticatedEncryptionFilter::BlockPaddingScheme::ZEROS_PADDING
                                                                                 ));
        
        _outputLen = static_cast<int>(cs.TotalPutLength());
        
    } catch (std::exception& e) {
        ErrorLog <<"encrypt exception"<<e.what();
        return -1;
    }

    
    
    std::string ss = "";
    
    for(int ii = 0; ii < _outputLen; ii++){
        
        ss += std::to_string(static_cast<unsigned char>(_output[ii])) + " ";
        
    }
    
    InfoLog <<"encrypt:"<<ss;
    
    
    return 0;
    
}


int common::decrypt(const char* _input, const int& _inputLen, char* _output, int& _outputLen, const std::string& key, const std::string& iv)
{
    
    try {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryption((byte*)key.c_str(), key.length(), (byte*)iv.c_str());
        
        
        CryptoPP::ArraySink cs((byte*)_output, _outputLen);
        
        CryptoPP::ArraySource decryptor((byte*)_input, _inputLen, true,
                                        new CryptoPP::StreamTransformationFilter(decryption,
                                                                                 new CryptoPP::Redirector(cs),
                                                                                 CryptoPP::AuthenticatedEncryptionFilter::BlockPaddingScheme::ZEROS_PADDING
                                                                                 ));
        _outputLen = static_cast<int>(cs.TotalPutLength());
    } catch (std::exception& e) {
        ErrorLog <<"encrypt exception"<<e.what();
        return -1;
    }
    return 0;
}




std::string common::encrypt64coding(const std::string& str_in, const std::string& key, const std::string& iv)
{
    std::string str_out;
    CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption((byte*)key.c_str(), key.length(), (byte*)iv.c_str());
    
    
    CryptoPP::StringSource encryptor(str_in, true,
                                     new CryptoPP::StreamTransformationFilter(encryption,
                                                                              new CryptoPP::Base64Encoder(
                                                                                                          new CryptoPP::StringSink(str_out),
                                                                                                          false // do not append a newline
                                                                                                          ),
                                                                              CryptoPP::AuthenticatedEncryptionFilter::BlockPaddingScheme::ZEROS_PADDING
                                                                              )
                                     );
    
    
  
    
    return str_out;
}


std::string common::decrypt64coding(const std::string& str_in, const std::string& key, const std::string& iv)
{
    
    
    std::string str_out;
    CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryption((byte*)key.c_str(), key.length(), (byte*)iv.c_str());
    
    
    CryptoPP::StringSource decryptor(str_in, true,
                                     new CryptoPP::Base64Decoder(
                                                                 new CryptoPP::StreamTransformationFilter(decryption,
                                                                                                          new CryptoPP::StringSink(str_out), 
                                                                                                          CryptoPP::AuthenticatedEncryptionFilter::BlockPaddingScheme::ZEROS_PADDING
                                                                                                          )
                                                                 )
                                     );
    return str_out;
}

















