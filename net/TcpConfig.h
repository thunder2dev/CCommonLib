//
//  TcpConfig.hpp
//  marstcp
//
//  Created by meryn on 2017/10/10.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef TcpConfig_hpp
#define TcpConfig_hpp

#include <string>
#include <map>
#include <memory>
#include <functional>

namespace tinyxml2{
    class XMLDocument;
    class XMLElement;
    
}




namespace net {
    
    class TcpConfig: public std::enable_shared_from_this<TcpConfig>{
    public:
        TcpConfig(std::string _configFileName);
        
        int Load();
        
    public:
        std::string m_name;
        std::string m_ip;
        int m_port;
        
        
    private:
        int handleName(tinyxml2::XMLElement* _ele);
        int handleIp(tinyxml2::XMLElement* _ele);
        int handlePort(tinyxml2::XMLElement* _ele);
        
        
    private:
        
        std::string m_configFileName;
        std::shared_ptr<tinyxml2::XMLDocument> m_xmlDoc;

    protected:
        std::map<std::string, std::function<int(tinyxml2::XMLElement*)>> m_handlers;
        
        
    };
    
    
    
}



#endif /* TcpConfig_hpp */
