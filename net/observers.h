//
//  IObserver.h
//  marstcp
//
//  Created by meryn on 2017/10/13.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef IObserver_h
#define IObserver_h

class IConnectionCloseListener{
    
public:
    
    virtual void OnConnectionClosed() = 0;
    
};



#endif /* IObserver_h */
