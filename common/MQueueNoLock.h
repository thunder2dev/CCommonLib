//
//  MQueue.h
//  marstcp
//
//  Created by meryn on 2017/08/22.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef MQueueNoLock_h
#define MQueueNoLock_h //not to use

#include <cassert>
#include <memory>
#include <atomic>
#include <new>
#include "log/Log.h"

namespace common {
    
    template<typename T>
    class MQueueNoLock{
    public:
        const static int MAX_BUFFER_SIZE = 1000000;
        
        MQueueNoLock(const MQueueNoLock&) = delete;
        MQueueNoLock& operator=(const MQueueNoLock&) = delete;
        MQueueNoLock(MQueueNoLock&&) = delete;
        
        MQueueNoLock(int sz = MAX_BUFFER_SIZE):size(sz), head(0), tail(0), count(0){
            
            this->arr = reinterpret_cast<T*>(new(std::nothrow) char[sizeof(T) * size]);
            this->flagArr = new(std::nothrow) std::atomic<int>[size];
            assert(this->arr != nullptr);
            assert(this->flagArr != nullptr);
            std::memset(this->arr, 0, sizeof(T) * size);
            std::memset(this->flagArr, 0, size);
        }
        
        bool EnQueue(const T& item){
            
            int curr_tail;
            int empty_code;
            int repeat = 0;
            
            do{
                if(count >= size - 1){
                    InfoLog << "en disable head:"<< this->head << " tail:"<<this->tail <<" count:"<<count;
                    return false;
                }
                
                if(repeat++ > 100){
                    InfoLog <<"stop";
                };
                
                empty_code = 0;
                curr_tail = tail;
            }while (!this->flagArr[curr_tail].compare_exchange_weak(empty_code, 1));
            
            int new_tail = (curr_tail + 1) % (size - 1);
            
            this->tail.compare_exchange_weak(curr_tail, new_tail);
            
            this->arr[curr_tail] = item;
            
            this->count++;
            
            __asm__ __volatile__("" ::: "memory");
            
            this->flagArr[curr_tail] = 2;
            
            return true;
        }
        
        
        bool DeQueue(T& item){
            
            int curr_head;
            int avail_code;
            int repeat = 0;
            
            do{
                int cnt = count;
                if(cnt <= 0){
                    InfoLog <<"empty head:" << this->head <<" tail:" <<this->tail << " count:"<<cnt;
                    return false;
                }
                
                if(repeat++ > 100){
                    InfoLog <<"stop";
                };
                
                avail_code = 2;
                curr_head = this->head;
            }while (!this->flagArr[curr_head].compare_exchange_weak(avail_code, 3));
            
            item = this->arr[curr_head];
            
            int new_head = (curr_head + 1) % (size - 1);
            
            this->head.compare_exchange_weak(curr_head, new_head);
            
            this->count--;
            
            __asm__ __volatile__("" ::: "memory");
            
            this->flagArr[curr_head] = 0;
            
            return true;
            
        }
        
    private:
        
        int size;
        std::atomic<int> head, tail, count;
        std::atomic<int> *flagArr;    //0: empty 1:en-ing 2:en-ed 3 del-ing
        
        T* arr;
    };
}




#endif /* MQueue_h */


















