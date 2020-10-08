#include "average_counter.h"
#include "sum_job.h"
#include "thread_pool.h"

SumJob::SumJob(AverageCounter& AverageCounter, ThreadPool& pool) : counter { AverageCounter }, pool { pool }
{
}

void SumJob::run()
{
    double first; 
    double second;
    {
        std::scoped_lock<std::mutex> lock(counter.getMutex());
        if (!counter.getValues(first, second))
            return;
    }

    auto sum = first + second;
    {
        std::scoped_lock<std::mutex> lock(counter.getMutex());
        counter.pushSum(sum);
    }

    if (counter.valuesSize() >= 2)
    {
        pool.push(std::make_unique<SumJob>(counter, pool));
    }
}
