//
//  ProtoConnectionExt.hpp
//  marstcp
//
//  Created by meryn on 2017/09/27.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef ProtoConnectionExt_hpp
#define ProtoConnectionExt_hpp

#include "net/ConnectionExt.h"
#include "ProtoCommon.h"
#include <chrono>
#include <memory>
#include <functional>
#include <map>
#include "log/Log.h"


namespace net {
    
    const std::chrono::milliseconds RspCallback_ExpireDuration = std::chrono::milliseconds(1000);
    
    class ProtoConnectionExt;
    
    class RspCallbackUnit{
        
    public:
        RspCallbackUnit(std::function<ProtoResponseCallback> _callback)
        :m_callback(_callback), m_expiredTP(std::chrono::system_clock::now() + RspCallback_ExpireDuration )
        {
            
        }
        
        
    private:
        std::function<ProtoResponseCallback> m_callback;
        std::chrono::time_point<std::chrono::system_clock> m_expiredTP;
        
        friend ProtoConnectionExt;
    };
    
    
    class ProtoConnectionExt: public ConnectionExt{
    
    public:
        int AddRspCallback(int64_t _seq, std::function<ProtoResponseCallback> _cb){
            if(m_rspCallbacks.count(_seq) > 0){
                ErrorLog << "rsp callback already has a seq:" << _seq;
                return -1;
            }
            
            m_rspCallbacks[_seq] = std::make_shared<RspCallbackUnit>(_cb);
            return 0;
        }
        
        bool HasRspCallback(int64_t _seq){
            
            return m_rspCallbacks.count(_seq) > 0;
            
        }
        
        std::function<ProtoResponseCallback> PopRspHandler(int64_t _seq){
            
            std::shared_ptr<RspCallbackUnit> unit = m_rspCallbacks[_seq];
            m_rspCallbacks.erase(_seq);
            return unit->m_callback;
        }
        
        /*
        int updateExpireTimepoint(){
            
            std::chrono::time_point<std::chrono::system_clock>
            
            for(const auto& kv: m_rspCallbacks){
                if(kv)
                
                
            }
            
            
            return 0;
        }*/
        
        
    private:
        std::map<int64_t, std::shared_ptr<RspCallbackUnit>> m_rspCallbacks;
        
    };
}




#endif /* ProtoConnectionExt_hpp */
