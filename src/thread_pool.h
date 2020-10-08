#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <type_traits>
#include <typeinfo>
#include <iostream>

#include "job.h"


class ThreadPool
{
public:
    ThreadPool(std::size_t threadsAmount = (std::thread::hardware_concurrency() - 1))
    {
        if (threadsAmount < 1)
            threadsAmount = 1;

        std::cout << "Threads amount: " << threadsAmount << std::endl;
        threads.reserve(threadsAmount);
        for (size_t i = 0; i != threadsAmount; ++i)
        {
            threads.emplace_back(std::thread(&ThreadPool::loop, this));
        }
    }

    ~ThreadPool()
    {
        terminate = true;
        condVar.notify_all();

        for (auto& thread : threads)
            thread.join();
    }

    template <typename T>
    void push(std::unique_ptr<T>&& job)
    {
        {
            std::scoped_lock<std::mutex> lock(mutex);
            jobQueue.emplace(std::move(job));
        }

        condVar.notify_one();
    }

    template <typename Iterator>
    void push(const Iterator& begin, const Iterator& end)
    {
        {
            std::scoped_lock<std::mutex> lock(mutex);
            std::for_each(begin, end, [&queue = jobQueue](auto& job) { queue.emplace(std::move(job)); });
        }
        condVar.notify_all();
    }

private:
    std::mutex mutex;
    std::condition_variable condVar;
    std::atomic<bool> terminate = false;

    std::vector<std::thread> threads;
    std::queue<std::unique_ptr<Job>> jobQueue;

    void loop()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (jobQueue.empty() || !terminate)
                condVar.wait(lock);

            if (terminate)
                break;

            if (jobQueue.empty())
                continue;

            auto job = std::move(jobQueue.front());
            jobQueue.pop();
            lock.unlock();
            job->run();
        }
    }
};
