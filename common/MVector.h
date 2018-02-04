//
//  MVector.h
//  marstcp
//
//  Created by meryn on 2017/08/23.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef MVector_h
#define MVector_h

#include <vector>
#include "common/Noncopyable.h"
#include <pthread.h>
#include "common/RWLock.h"


namespace common {
    template<typename _Tp>
    class MVector: Noncopyable{
        
    public:
        using vec_type = std::vector<_Tp>;
        using value_type = typename vec_type::value_type;
        using iterator = typename vec_type::iterator;
        using size_type = typename vec_type::size_type;
        
        template <class _Func>
        void for_each_read(_Func __f){
            auto guard = this->lock.read_guard();
            for(auto itr = vec.begin(); itr != vec.end(); ++itr){
                __f(*itr);
            }
        }
        
        iterator find(const _Tp& __x ){
            auto guard = this->lock.read_guard();
            return std::find(vec.begin(), vec.end(), __x);
        }
        
        void erase(const _Tp& __x){
            auto guard = this->lock.write_guard();
            vec.erase(__x);
        }
        
        void erase(iterator __pos){
            auto guard = this->lock.write_guard();
            vec.erase(__pos);
        }
        
        void push_back(const _Tp& __x ){
            auto guard = this->lock.write_guard();
            vec.push_back(__x);
        }
        
        void clear(){
            auto guard = this->lock.write_guard();
            vec.clear();
        }
        
        size_type size(){
            auto guard = this->lock.read_guard();
            return vec.size();
        }
        
        
        value_type& get(int idx){
            auto guard=lock.read_guard();
            return vec[idx];
        }
        
        void set(int idx, const value_type& val){
            auto guard = lock.write_guard();
            vec[idx] = val;
        }
        
        
    private:
        mutable RWLock lock;
        
        std::vector<_Tp> vec;
        
    };
    
}



#endif /* MVector_h */





















