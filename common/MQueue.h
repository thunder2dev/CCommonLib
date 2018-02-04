//
//  MQueue.h
//  marstcp
//
//  Created by meryn on 2017/08/23.
//  Copyright © 2017 marsladder. All rights reserved.
//

#ifndef MQueue_h
#define MQueue_h

#include <mutex>
#include <queue>


namespace common {
    
    template<typename T>
    class MQueue{
    public:
        MQueue(const MQueue&) = delete;
        MQueue(MQueue&&) = delete;
        MQueue& operator=(const MQueue&) = delete;
        MQueue& operator=(MQueue&&) = delete;
        
        MQueue() = default;
        
        void Enqueue(T&& val){
            std::lock_guard<std::mutex> guard(mutex);
            queue.push(std::forward<T>(val));
        }
        
        
        bool Dequeue(T& val){
            std::lock_guard<std::mutex> guard(mutex);
            if(queue.empty()){
                //InfoLog << "dequeue false";
                return false;
            }
            
            val = queue.front();
            queue.pop();
            
            return true;
        }
        

    private:
        std::queue<T> queue;
        std::mutex mutex;
        

    };
    
    
    
    
}


#endif /* MQueue_h */
