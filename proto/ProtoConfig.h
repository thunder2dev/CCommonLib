//
//  ProtoConfig.hpp
//  marstcp
//
//  Created by meryn on 2017/10/10.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef ProtoConfig_hpp
#define ProtoConfig_hpp

#include "net/TcpConfig.h"


namespace net {
    class ProtoConfig: public TcpConfig{
    public:
        ProtoConfig(std::string _configFileName);
        
        int handleFileRoot(tinyxml2::XMLElement* _ele);
        
        
    public:
        std::string m_fileRoot;
        
    };
}

#endif /* ProtoConfig_hpp */
