//
//  Heap.hpp
//  marstcp
//
//  Created by meryn on 2017/09/08.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef Heap_hpp
#define Heap_hpp

#include <vector>
#include <sstream>

template<class _Ty, class _Container = std::vector<_Ty>, class _Pr = std::less<typename _Container::value_type>>
class Heap{
public:
    typedef typename _Container::value_type value_type;
    typedef typename _Container::size_type size_type;
    typedef typename _Container::reference reference;
    typedef typename _Container::const_reference const_reference;
    
    Heap(): c(), comp(){}
    explicit Heap(const _Pr& _Pred): c(), comp(_Pred){}
    Heap(const _Pr& _Pred, const _Container& _Cont): c(_Cont), comp(_Pred){}
    
    template<class _Iter>
    Heap(_Iter _First, _Iter _Last) : c(_First, _Last), comp(){
        std::make_heap(c.begin(), c.end(), comp);
    }
    
    /*template<class _Iter>
    Heap(_Iter _First, _Iter _Last, const _Pr& _Pred): c(_First, _Last), comp(_Pred){
        std::make_heap(c.begin(), c.end(), comp);
    }*/
    
    bool empty() const{
        return c.empty();
    }
    
    size_type size() const{ return c.size(); }
    
    const_reference top() const{ return c.front(); }
    
    reference top() { return c.front(); }
    
    void push(const value_type& _Val){
        c.push_back(_Val);
        std::push_heap(c.begin(), c.end(), comp);
    }
    
    void pop(){
        std::pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }
    
    std::string print(){
        std::ostringstream ss;
        for(auto& ele : c){
            ss << ele << " ";
        }
        return ss.str();
        
    }
    
    
    template<class _Iter>
    _Iter find(const_reference _Val){
        return c.find(_Val);
    }

    void erase(const_reference _Val){
        auto iter = std::find(c.begin(), c.end(), _Val);
        if(iter == c.end()){
            return;
        }
        if(c.end() == c.begin()){
            return;
        }
        
        std::swap(*iter, *(c.end()-1));
        c.pop_back();
        adjustFromIndex(static_cast<int>(iter - c.begin()));
    }
    
private:
    void adjustFromIndex(int _idx){
        
        int idx = _idx;
        int n = static_cast<int>(c.size() - 1);
        int child = idx;
        for(;idx*2 <= n;idx=child){
            child = idx * 2 + 1;
            if(comp(c[child], c[child + 1]) > 0){
                child+=1;
            }
            if(comp(c[idx], c[child]) > 0){
                std::swap(c[idx], c[child]);
            }else{
                break;
            }
        }
    }
    
    
protected:
    _Container c;
    _Pr comp;
    
    
    
    
};

/*
template<class T>
Heap<T>::Heap(int size){
    
}
*/






#endif /* Heap_hpp */







