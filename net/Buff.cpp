//
//  Buff.cpp
//  marstcp
//
//  Created by meryn on 2017/09/19.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "Buff.h"


net::Buff::Buff(int32_t _size)
:m_size(_size),
m_startIdx(kBuffHeadSize),
m_endIdx(kBuffHeadSize),
data(_size){
    
}


net::Buff::Buff()
:net::Buff(kTcpReadBuffSize*2)
{    
}



void net::Buff::Reset(){
    m_startIdx = m_endIdx = kBuffHeadSize;
}


int net::Buff::SetHaveConsumed(int32_t n){
    
    if(Length() < n){
        ErrorLog << "size count err:"<< this->Length();
        return -1;
    }
    
    m_startIdx += n;
    return 0;
}


int net::Buff::SetHaveAppended(int32_t n){
    
    if(AvailCount() < n){
        ErrorLog << "avail count err:"<< this->AvailCount();
        return -1;
    }
    
    m_endIdx += n;
    return 0;
}

int net::Buff::AppendUint(const uint32_t& num){

    if(this->AvailCount() < 4){
        ErrorLog << "avail count err:"<< this->AvailCount();
        return -1;
    }
    
    data[m_endIdx] = num & 0xFF;
    data[m_endIdx + 1] = (num >> 8) & 0xFF;
    data[m_endIdx + 2] = (num >> 16) & 0xFF;
    data[m_endIdx + 3] = (num >> 24) & 0xFF;
    
    m_endIdx += 4;
    
    return 0;
    
}


int net::Buff::PrependInt64(const int64_t& num){
    
    if(this->m_startIdx < 8){
        ErrorLog << "prepent avail err:"<< this->m_startIdx;
        return -1;
    }
    
    //int nNum = htonl(num);
    
    data[m_startIdx - 8] = num & 0xFF;
    data[m_startIdx - 7] = (num >> 8) & 0xFF;
    data[m_startIdx - 6] = (num >> 16) & 0xFF;
    data[m_startIdx - 5] = (num >> 24) & 0xFF;
    data[m_startIdx - 4] = (num >> 32) & 0xFF;
    data[m_startIdx - 3] = (num >> 40) & 0xFF;
    data[m_startIdx - 2] = (num >> 48) & 0xFF;
    data[m_startIdx - 1] = (num >> 56) & 0xFF; 
    
    
    m_startIdx -= 4;
    
    return 0;
    
    
}

int net::Buff::ReadInt64(int64_t& num){
    
    if(this->Length() < 4){
        ErrorLog << "size err:"<< this->Length();
        return -1;
    }
    
    int64_t a1 = static_cast<int64_t>(static_cast<unsigned char>(data[m_startIdx]));
    int64_t a2 = static_cast<int64_t>(static_cast<unsigned char>(data[m_startIdx + 1])) << 8;
    int64_t a3 = static_cast<int64_t>(static_cast<unsigned char>(data[m_startIdx + 2])) << 16;
    int64_t a4 = static_cast<int64_t>(static_cast<unsigned char>(data[m_startIdx + 3])) << 24;
    int64_t a5 = static_cast<int64_t>(static_cast<unsigned char>(data[m_startIdx + 4])) << 32;
    int64_t a6 = static_cast<int64_t>(static_cast<unsigned char>(data[m_startIdx + 5])) << 40;
    int64_t a7 = static_cast<int64_t>(static_cast<unsigned char>(data[m_startIdx + 6])) << 48;
    int64_t a8 = static_cast<int64_t>(static_cast<unsigned char>(data[m_startIdx + 7])) << 56;
    
    num = a1 | a2 | a3 | a4 | a5 | a6 | a7 | a8;
    
    m_startIdx += 4;
    
    return 0;
    
}




int net::Buff::PrependUnit(const uint32_t& num){
    
    if(this->m_startIdx < 4){
        ErrorLog << "prepent avail err:"<< this->m_startIdx;
        return -1;
    }
    
    //int nNum = htonl(num);
    
    data[m_startIdx - 4] = num & 0xFF;
    data[m_startIdx - 3] = (num >> 8) & 0xFF;
    data[m_startIdx - 2] = (num >> 16) & 0xFF;
    data[m_startIdx - 1] = (num >> 24) & 0xFF;
    
    m_startIdx -= 4;
    
    return 0;
    
}




int net::Buff::ReadUint(uint32_t& num){
    
    if(this->Length() < 4){
        ErrorLog << "size err:"<< this->Length();
        return -1;
    }
    
    uint32_t a1 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx]));
    uint32_t a2 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx + 1])) << 8;
    uint32_t a3 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx + 2])) << 16;
    uint32_t a4 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx + 3])) << 24;
    
    num = a1 | a2 | a3 | a4;
    
    m_startIdx += 4;
    
    return 0;
    
}


int net::Buff::AppendInt(const int32_t &num){
    
    if(this->AvailCount() < 4){
        ErrorLog << "avail count err:"<< this->AvailCount();
        return -1;
    }
    
    //int nNum = htonl(num);
    
    data[m_endIdx] = num & 0xFF;
    data[m_endIdx + 1] = (num >> 8) & 0xFF;
    data[m_endIdx + 2] = (num >> 16) & 0xFF;
    data[m_endIdx + 3] = (num >> 24) & 0xFF;
    
    m_endIdx += 4;
    
    return 0;
}

int net::Buff::PeekInt(int32_t& num){
    
    if(this->Length() < 4){
        ErrorLog << "avail count err:"<< this->AvailCount();
        return -1;
    }
    
    int a1 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx]));
    int a2 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx + 1])) << 8;
    int a3 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx + 2])) << 16;
    int a4 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx + 3])) << 24;
    
    num = a1 | a2 | a3 | a4;
    
    return 0;
}

int net::Buff::PrependInt(const int32_t& num){
    
    if(this->m_startIdx < 4){
        ErrorLog << "prepent avail err:"<< this->m_startIdx;
        return -1;
    }
    
    //int nNum = htonl(num);
    
    data[m_startIdx - 4] = num & 0xFF;
    data[m_startIdx - 3] = (num >> 8) & 0xFF;
    data[m_startIdx - 2] = (num >> 16) & 0xFF;
    data[m_startIdx - 1] = (num >> 24) & 0xFF;
    
    m_startIdx -= 4;
    
    return 0;
}



int net::Buff::ReadInt(int32_t &num){
    
    if(this->Length() < 4){
        ErrorLog << "size err:"<< this->Length();
        return -1;
    }
    
    int a1 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx]));
    int a2 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx + 1])) << 8;
    int a3 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx + 2])) << 16;
    int a4 = static_cast<int>(static_cast<unsigned char>(data[m_startIdx + 3])) << 24;
    
    num = a1 | a2 | a3 | a4;
    
    m_startIdx += 4;
    
    return 0;
}


int net::Buff::AppendBytes(const char *str, int len){
    
    if(this->AvailCount() < len){
        ErrorLog << "avail count err:"<< this->AvailCount();
        return -1;
    }
    
    memcpy(this->data.data() + m_endIdx, str, len);
    
    m_endIdx += len;
    
    return 0;
}


int net::Buff::ReadBytes(char *str, int len){
    
    if(this->Length() < len){
        ErrorLog << "size err:"<< this->Length()<<" need size:"<<len;
        return -1;
    }
    
    memcpy(str, this->data.data() + m_startIdx, len);
    
    m_startIdx += len;
    
    return 0;
}

void net::Buff::Rearrange(){
    
    memmove(this->data.data(), this->data.data() + m_startIdx, this->Length());
    
    m_endIdx = this->Length();
    m_startIdx = 0;
    
}












