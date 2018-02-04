//
//  Connector.hpp
//  marstcp
//
//  Created by meryn on 2017/09/12.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Connector_hpp
#define Connector_hpp

#include <string>
#include <memory>
#include <iostream>
#include "Connection.h"
#include "callbacks.h"

namespace common {
    class TimerEvent;
}

namespace net {

class EvLoop;
    
class Connector: public Connection{
    
public:
    Connector(std::shared_ptr<EvLoop> _loop, std::string _ip, int _port, std::string name="");
    
    void Connect();
    int ConnectOnceTry(std::shared_ptr<common::TimerEvent> ev);
    
    
    
    void SetConnectedCallback(std::function<common::ConnectedCallback> _cb){ m_connectedCallback = _cb; }
    
    
private:
    std::string m_name;
    std::function<common::ConnectedCallback> m_connectedCallback;
    
};

std::ostream& operator<<(std::ostream& os, const Connector& con);
    


    
    
    

}


#endif /* Connector_hpp */














