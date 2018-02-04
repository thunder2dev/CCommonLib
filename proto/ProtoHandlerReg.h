//
//  HandlerReg.h
//  marstcp
//
//  Created by meryn on 2017/09/25.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Protobuf_HandlerReg_h
#define Protobuf_HandlerReg_h

#include <memory>

namespace net {
    class ProtobufDispatcher;
    void RegAllHandlers(std::shared_ptr<net::ProtobufDispatcher> _dispatch);
}


#endif /* HandlerReg_h */
