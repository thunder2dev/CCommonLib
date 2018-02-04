//
//  MMap.hpp
//  marstcp
//
//  Created by meryn on 2017/08/22.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef MMap_hpp
#define MMap_hpp

#include <memory>
#include <utility>
#include <map>
#include "RWLock.h"

namespace common {
    template<typename _Key, typename _Val >
    class MMap{
    private:
        std::map<_Key, _Val> map;
        mutable RWLock lock;
    public:
        using map_type=std::map<_Key,_Val>;
        using key_type=typename map_type::key_type;
        using value_type=typename map_type::value_type;
        using mapped_type=typename map_type::mapped_type;
        using size_type=typename map_type::size_type;
        using iterator=typename map_type::iterator;
        using const_iterator=typename map_type::const_iterator;
        
        MMap() = default;
        MMap(const MMap& mm) = delete;
        MMap& operator=(const MMap& mm) = delete;
        MMap(std::initializer_list<value_type> _l):map(_l){
        }
        
        size_type size() const noexcept{
            auto guard = lock.read_guard();
            return map.size();
        }
        
        size_type count(const key_type& _x) const {
            auto guard=lock.read_guard();
            return map.count(_x);
        }
        
        iterator end() noexcept{
            auto guard = lock.read_guard();
            return map.end();
        }
        
        const_iterator end() const noexcept{
            auto guard = lock.read_guard();
            return map.end();
        }
        
        
        template<typename... _Args>
        std::pair<iterator, bool> emplace(_Args&&... _args){
            auto guard = lock.write_guard();
            return map.emplace(std::forward<_Args>(_args)...);
        }
        
        /*
        iterator find(const key_type& _x){
            auto guard = lock.read_guard();
            return map.find(_x);
        }*/
        
        void erase(const key_type& __x){
            auto guard = this->lock.write_guard();
            map.erase(__x);
        }
        
        iterator erase(iterator _position){
            auto guard=lock.write_guard();
            return map.erase(_position);
        }
        
        void clear() noexcept{
            auto guard=lock.write_guard();
            map.clear();
        }
        
        /*
        mapped_type& operator[](key_type&& _k){
            auto guard=lock.write_guard();
            return map[std::forward<key_type>(_k)];
        }*/
        
        bool exists(const key_type& _k){
            auto guard=lock.read_guard();
            return map.count(_k) > 0;
        }
        
        bool get(const key_type& _k, mapped_type& _v){
            auto guard=lock.read_guard();
            
            if(map.count(_k) <= 0){
                return false;
            }
            
            _v = map[_k];
            
            return true;
        }
        
        void set(const key_type& _k, mapped_type& _v){
            auto guard = lock.write_guard();
            map[_k] = _v;
        }
        
        void set(const key_type& _k, mapped_type&& _v){
            auto guard = lock.write_guard();
            map[_k] = std::move(_v);
        }
        
    };
}















#endif /* MMap_hpp */














