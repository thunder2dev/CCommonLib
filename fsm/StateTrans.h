//
//  StateTrans.h
//  marstcp
//
//  Created by meryn on 2017/09/29.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef StateTrans_h
#define StateTrans_h

#include <cstdint>



namespace fsm {
    class StateTrans{
        
    public:
        
        StateTrans(int32_t _src, int32_t _dst, int32_t _trigger, bool _isReentry)
        :m_src(_src), m_dst(_dst), m_isReentry(_isReentry){
            
        }
        
        int32_t GetDstId(){ return m_dst;}
        int32_t GetSrcId(){ return m_src;}
        bool GetReentry() { return m_isReentry;}
        
    private:
        int32_t m_src;
        int32_t m_dst;
        bool m_isReentry;
        
        
        
    };
}




#endif /* StateTrans_h */
