//
//  SMFunc.hpp
//  marstcp
//
//  Created by meryn on 2017/09/28.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef SMFunc_hpp
#define SMFunc_hpp

#include <functional>
#include <string>
#include "log/Log.h"



namespace fsm {

    
    template <typename T, T t> class SMFunc;
    
    template<typename T, typename ...Args, int(T::*mf)(Args...)>
    class SMFunc<int (T::*)(Args...), mf>{
        
    public:
        SMFunc(std::string _name=""): m_name(_name){}
        
        int call(std::shared_ptr<T> ptr, Args &&... args){
            if(ptr.get() == nullptr){
                ErrorLog <<"func obj null:"<<m_name;
                return -1;
            }
            
            return ((*ptr).*mf)(std::forward<Args>(args)...);
        }
        
    private:
        std::string m_name;
    };

}



#endif /* SMFunc_hpp */






















