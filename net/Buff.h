//
//  Buff.hpp
//  marstcp
//
//  Created by meryn on 2017/08/31.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Buff_hpp
#define Buff_hpp

#include "common/common.h"
#include "common/Noncopyable.h"
#include "log/Log.h"
#include <vector>
#include <cstdint>


namespace net {
    
const int kTcpReadBuffSize = 10240;
const int kMaxMsgNumInSendBuff = 10;
const int kTcpWriteBuffSize = kTcpReadBuffSize*kMaxMsgNumInSendBuff;
const int kBuffHeadSize = 20;
const int kTcpChunkBuffSize = common::FILE_TRANSFER_PACK_SIZE + kBuffHeadSize;
    
    
class Buff: public common::Noncopyable{

public:
    
    Buff();
    Buff(int32_t _size);
    
    char* GetReadableCArr(){ return data.data() + m_startIdx; }
    char* GetWritableCArr(){ return data.data() + m_endIdx; }
    int32_t AvailCount() { return m_size - m_endIdx;}
    int32_t Length() { return m_endIdx - m_startIdx; }
    
    int SetHaveConsumed(int32_t n);
    int SetHaveAppended(int32_t n);
    
    int AppendUint(const uint32_t& num);
    int PrependUnit(const uint32_t& num);
    int ReadUint(uint32_t& num);
    int AppendInt(const int32_t& num);
    int PrependInt(const int32_t& num);
    
    int PeekInt(int32_t& num);
    int ReadInt(int32_t& num);
    int AppendShort(const int16_t& num);
    int PrependShort(const int16_t& num);
    int ReadShort(int16_t& num);
    int AppendInt64(const int64_t& num);
    int PrependInt64(const int64_t& num);
    int ReadInt64(int64_t& num);
    int AppendBytes(const char* str, int len);
    int ReadBytes(char* str, int len);
    
    
    void Rearrange();
    void Reset();
    
private:
    int32_t m_startIdx;
    int32_t m_endIdx;
    std::vector<char> data;
    int32_t m_size;

};

}




#endif /* Buff_hpp */





