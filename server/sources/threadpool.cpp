#include "threadpool.h"



ThreadPool::ThreadPool(std::size_t size)
: tp_stop(false), tp_size(size)
{
    if(0 == size){
        throw std::invalid_argument("ThreadPool must have least one thread.\n"); 
    }
    for(int i = 0; i < tp_size; ++i){
        tp_pool.emplace_back([this]{
            while(true){
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->tp_basic_mutex);
                    this->tp_condition.wait(lock,[this]{
                        return this->tp_stop ||
                                !this->tp_tasks.empty();
                    });


                    if(this->tp_stop && this->tp_tasks.empty()){
                        return;
                    }

                    task = std::move(this->tp_tasks.front());
                    this->tp_tasks.pop();
                }
                task();
            }
        });
    }
}



ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock(tp_basic_mutex);
        tp_stop = true;
    }

    tp_condition.notify_all();
    for(auto & t : tp_pool){
        if(t.joinable()) t.join();
    }

}