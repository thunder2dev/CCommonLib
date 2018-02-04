   //
//  ProtoConfig.cpp
//  marstcp
//
//  Created by meryn on 2017/10/10.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "ProtoConfig.h"
#include "common/tinyxml2.h"

using namespace tinyxml2;

net::ProtoConfig::ProtoConfig(std::string _configFileName)
:net::TcpConfig(_configFileName)
{
    m_handlers["fileRoot"] = [this](XMLElement* _ele)-> int{return this->handleFileRoot(_ele);};
    
}


int net::ProtoConfig::handleFileRoot(tinyxml2::XMLElement* _ele){
    
    const char* root = _ele->GetText();
    assert(root);
    m_fileRoot = root;
    
    return 0;
}







































