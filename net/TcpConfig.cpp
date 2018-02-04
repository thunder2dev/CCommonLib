//
//  TcpConfig.cpp
//  marstcp
//
//  Created by meryn on 2017/10/10.
//  Copyright © 2017 marsladder. All rights reserved.
//

#include "TcpConfig.h"
#include "common/Utils.h"
#include "common/tinyxml2.h"

using namespace tinyxml2;


net::TcpConfig::TcpConfig(std::string _configFileName)
:m_xmlDoc(std::make_shared<tinyxml2::XMLDocument>()),
m_configFileName(_configFileName)
{

    m_handlers["name"] = [this](XMLElement* _ele)-> int{return this->handleName(_ele);};
    m_handlers["ip"] = [this](XMLElement* _ele)-> int{return this->handleIp(_ele);};
    m_handlers["port"] = [this](XMLElement* _ele)-> int{return this->handlePort(_ele);};
    
}

int net::TcpConfig::handleName(tinyxml2::XMLElement* _ele){
    
    const char* text = _ele->GetText();
    assert(text);
    m_name = text;
    
    return 0;
}


int net::TcpConfig::handleIp(tinyxml2::XMLElement* _ele){
    
    const char* text = _ele->GetText();
    assert(text);
    m_ip = text;
    
    return 0;
}


int net::TcpConfig::handlePort(tinyxml2::XMLElement* _ele){
    
    XMLError ret = _ele->QueryIntText(&m_port);
    assert(ret == XML_SUCCESS);
    
    return 0;
}


int net::TcpConfig::Load(){
    
    std::string fullpath = common::GetConfigDir() + "/" + m_configFileName;
    
    XMLError ret = m_xmlDoc->LoadFile(fullpath.c_str());
    assert(ret == XML_SUCCESS);
    
    XMLNode* pRoot = m_xmlDoc->FirstChild();
    assert(pRoot != nullptr);
    
    for(auto iter: m_handlers){
        
        std::string tagName = iter.first;
        XMLElement* ele = pRoot->FirstChildElement(tagName.c_str());
        if(ele == nullptr){
            continue;
        }
        
        assert(iter.second(ele) == 0);
        
    }
    
    return 0;
}































