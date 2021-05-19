/*
 * @FilePath: /simtochat/util/src/ThreadPool.cpp
 * @Author: CGL
 * @Date: 2021-05-13 22:53:07
 * @LastEditors: CGL
 * @LastEditTime: 2021-05-19 21:51:14
 * @Description: 
 */
#include "ThreadPool.h"
#include <algorithm>

ThreadPool::ThreadPool(unsigned short size)
    : m_stoped(false)
{
    m_idleNum = std::max<unsigned short>(size, 1u);

    for (unsigned short i = 0; i < size; ++i)
    {
        m_poll.emplace_back(
            [this]
            {
                while (!this->m_stoped)
                {
                    Task task;

                    // Control the life cycle of lock.
                    {
                        std::unique_lock<std::mutex> lock(this->m_lock);

                        // Block the thread when no task is available.
                        this->m_cvTask.wait(
                            lock,
                            [this]
                            {
                                return this->m_stoped.load() || !this->m_tasks.empty();
                            }
                        );
                        if (this->m_stoped && this->m_tasks.empty()) return;

                        // Get the task from the task queue.
                        task = std::move(this->m_tasks.front());
                        this->m_tasks.pop();
                    }
                    
                    // Do the task.
                    this->m_idleNum--;
                    task();
                    this->m_idleNum++;
                }
            }
        );
    }
}

ThreadPool::~ThreadPool()
{
    m_stoped.store(true);
    m_cvTask.notify_all();
    for (auto& thread : m_poll)
    {
        if (thread.joinable()) thread.join();
    }
}

unsigned short ThreadPool::getIdleCount()
{
    return m_idleNum;
}