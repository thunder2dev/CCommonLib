//
//  common.h
//  marstcp
//
//  Created by meryn on 2017/08/02.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef common_h
#define common_h

#include <string>

namespace common {
    const int MAX_POLL_WAIT_TIME_MS = 5000;
    const int  MAX_POLL_WAIT_EVENTS_NUM = 100000;
    
    
    const int FILE_MAX_SIZE = 1024 * 1024 * 5 * 8;
    const int  FILE_TRANSFER_PACK_SIZE = 1024 * 8 * 2;
    const int  FILE_TRANSFER_INIT_DURATION_MS = 50;
    const int  FILE_TRANSFER_MAX_DURATION_MS = 1000;
    
    
    extern const std::string SUBDIR_CONF;
    extern const std::string SUBDIR_RES;
    
    extern std::string ROOT_PATH;
    extern bool IS_DEBUG_MODE;
    
    
    const int FRAME_COUNT = 256;
    const int MAX_POLYS = 256;
    
}










#endif /* common_h */
