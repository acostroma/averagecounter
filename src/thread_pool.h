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

    template <typename Func, typename... Args>
    auto push(Func&& f, Args&&... args)
    {
        using RetType = std::invoke_result_t<Func, Args...>;
        auto task = std::make_shared<std::packaged_task<RetType()>>([&f, &args...]() { return f(std::forward<Args>(args)...); });

        {
            std::scoped_lock<std::mutex> lock(mutex);
            jobQueue.emplace([task]() { (*task)(); });
        }

        condVar.notify_one();

        return task->get_future();
    }
    
private:
    std::mutex mutex;
    std::condition_variable condVar;
    std::atomic<bool> terminate = false;

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobQueue;

    void loop()
    {
        while (true)
        {
            std::function<void()> job;

            {
                std::unique_lock<std::mutex> lock(mutex);
                if (jobQueue.empty() || !terminate)
                    condVar.wait(lock);

                if (terminate)
                    break;

                job = std::move(jobQueue.front());
                jobQueue.pop();
            }

            job();
        }
    }
};
