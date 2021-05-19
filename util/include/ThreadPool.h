/*
 * @FilePath: /simtochat/util/include/ThreadPool.h
 * @Author: CGL
 * @Date: 2021-05-13 22:52:57
 * @LastEditors: CGL
 * @LastEditTime: 2021-05-19 21:51:26
 * @Description: This file provides a thread pool utility class.
 */
#ifndef UTIL_INCLUDE_THREAD_POLL_H
#define UTIL_INCLUDE_THREAD_POLL_H

#include <future>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

class ThreadPool
{
public:

    /**
     * @author: CGL
     * @param size The capacity of the thread pool.
     * @description: Creates a thread pool and
     *  initializes the specified number of threads.
     */    
    ThreadPool(unsigned short size = 4);

    // Wait all threads to finish.
    virtual ~ThreadPool();

public:

    /**
     * @author: CGL
     * @param f The task method to execute.
     * @param args All parameters of the task method.
     * @return Return std::future of the task method.
     *  And you can use std::future<>::get() to get the return value of the task method.
     * @description:
     *  Commit a task to task queue and wake up one of the threads to excute when it is idle.
     */
    template<class F, class... Args>
    auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>
    {
        if (m_stoped.load())
            throw std::runtime_error("Commit when thread pool is stopped!");

        // Derive the type of value returned by the function.
        using rettype = decltype(f(args...));
        
        auto task = std::make_shared<std::packaged_task<rettype()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        {
            std::lock_guard<std::mutex> lock{ m_lock };

            // Enqueue.
            m_tasks.emplace(
                [task](){ (*task)(); }
            );
        }

        // Wake up a thread to execute.
        m_cvTask.notify_one();

        return task->get_future();
    }
    
    /**
     * @author: CGL
     * @return Return the number of idle thread.
     * @description: Get the number of idle thread.
     */    
    unsigned short getIdleCount();

protected:

    using Task = std::function<void()>;
    
    std::vector<std::thread> m_poll;
    std::queue<Task> m_tasks;
    std::mutex m_lock;
    std::condition_variable m_cvTask;
    std::atomic<bool> m_stoped;
    std::atomic<int> m_idleNum;

};

#endif // !UTIL_INCLUDE_THREAD_POLL_H