#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <functional>
#include <queue>
#include <exception>
#include <utility>

class ThreadPool{
    public:
        ThreadPool(std::size_t size);
        ~ThreadPool();

        template<typename Func, typename... Args>
        void Enqueue(Func && function, Args&&... args){
            auto task_wrapper = [function_copy = std::forward<Func>(function), ...args_copy = std::forward<Args>(args)]() mutable{
                function_copy(args_copy...);
            };

            {
                std::unique_lock<std::mutex> lock(tp_basic_mutex);
                tp_tasks.emplace(std::move(task_wrapper));
            }
            tp_condition.notify_one();
        }
        
    private:
        std::mutex tp_basic_mutex;
        std::vector<std::thread> tp_pool;
        std::queue<std::function<void()>> tp_tasks;
        std::condition_variable tp_condition;
        std::size_t tp_size;
        std::atomic<bool> tp_stop;
};