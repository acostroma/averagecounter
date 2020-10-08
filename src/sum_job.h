#pragma once

#include "thread_pool.h"
#include "job.h"

class SumJob : public Job
{
public:
    explicit SumJob(AverageCounter& AverageCounter, ThreadPool& pool);
    ~SumJob() = default;

    void run() override;

private:
    AverageCounter& counter;
    ThreadPool& pool;
};
