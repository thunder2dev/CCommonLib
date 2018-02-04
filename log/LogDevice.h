//
//  LogDevice.h
//  marstcp
//
//  Created by meryn on 2017/08/03.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef LogDevice_h
#define LogDevice_h


class LogDevice{
public:
    virtual void Print(int num, ... ) = 0;
    virtual std::mutex& GetMutex() = 0;
    virtual std::ostream& GetStream() = 0;
};



#endif /* LogDevice_h */
