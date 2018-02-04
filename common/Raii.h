//
//  Raii.hpp
//  marstcp
//
//  Created by meryn on 2017/08/18.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Raii_hpp
#define Raii_hpp

#include <type_traits>
#include <functional>
#include <cassert>
#include "log/Log.h"


namespace  common{
    
    template<typename T>
    class no_const{
    public:
        using type = typename std::conditional<std::is_const<T>::value, typename std::remove_const<T>::type, T>::type;
    };
    
    class raii{
        
        using Handler = std::function<void()>;
        
    public:
        raii(const raii&) = delete;
        raii& operator=(const raii&) = delete;
        
        raii(raii&& rv)noexcept:releaseOff(rv.releaseOff), releaseHandler(rv.releaseHandler){
            rv.releaseOff = true;
            InfoLog <<"right val";
        }
        
        explicit raii(Handler _acquireHandler, Handler _releaseHandler) noexcept:releaseOff(false), releaseHandler(_releaseHandler){
            _acquireHandler();
        }
        
        ~raii() noexcept{
            if(!releaseOff){
                releaseHandler();
            }

        }
        
    private:
        Handler releaseHandler;
        bool releaseOff;
    };
    
    
    
    template<typename RES, typename METHOD_ACQ, typename METHOD_REL>
    raii make_raii(RES& obj, METHOD_ACQ acq, METHOD_REL rel) noexcept{
        static_assert(std::is_class<RES>::value, "obj is not an object");
        static_assert(std::is_member_function_pointer<METHOD_ACQ>::value, "acq is not member function");
        static_assert(std::is_member_function_pointer<METHOD_REL>::value, "rel is not member function");
        assert(nullptr != acq && nullptr != rel);
        
        auto obj_addr = std::addressof(const_cast<typename no_const<RES>::type&>(obj));
        
        return raii(std::bind(acq, obj_addr), std::bind(rel, obj_addr));
        
    }
    
    
    
    
}

#endif /* Raii_hpp */











