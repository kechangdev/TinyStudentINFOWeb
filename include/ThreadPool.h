//
// Created by KeChang on 5/1/2025.
//
// include/ThreadPool.h


#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include <future>

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>;

    // 析构函数，停止所有线程
    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

inline ThreadPool::ThreadPool(size_t threads)
    : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(
            [this] {
                while (true) {
                    std::function<void()> task;

                    {   // 获取任务
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    // 执行任务
                    task();
                }
            }
        );
    }
}

// enqueue 函数实现
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type> {
    using return_type = typename std::invoke_result<F, Args...>::type;

    // 创建一个包装任务，使用 std::packaged_task 包装函数和参数
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    // 获取未来对象
    std::future<return_type> res = task->get_future();

    {   // 添加任务到队列
        std::unique_lock<std::mutex> lock(queue_mutex);

        // 不允许在停止后添加任务
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });
    }

    // 通知一个等待的线程
    condition.notify_one();
    return res;
}

// 析构函数实现
inline ThreadPool::~ThreadPool() {
    {   // 设置停止标志
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    // 通知所有线程
    condition.notify_all();

    // 等待所有线程完成
    for (std::thread &worker: workers)
        if (worker.joinable())
            worker.join();
}


#endif //THREADPOOL_H
