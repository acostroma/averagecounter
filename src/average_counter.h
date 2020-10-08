#pragma once

#include "thread_pool.h"
#include "job.h"

class AverageCounter
{
public:
    static void count(AverageCounter& counter, ThreadPool& pool);

    AverageCounter(const std::vector<double>& numberValues);
    ~AverageCounter() = default;

    std::mutex& getMutex();
    double getResult();

    void pushSum(double sum);
    size_t valuesSize() const;
    bool getValues(double& first, double& second);

private:
    const size_t initialSize;
    std::queue<double> values;
    size_t sumsDone = 0;
    std::mutex mutex;
    bool ready;
    double average;
};
