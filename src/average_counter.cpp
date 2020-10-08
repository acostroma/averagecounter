#include "average_counter.h"
#include "sum_job.h"

#include <iostream>


AverageCounter::AverageCounter(const std::vector<double>& numberValues) : initialSize{ numberValues.size() }
{
    assert(initialSize > 1);
    for (auto& value : numberValues)
        values.push(value);
}

std::mutex& AverageCounter::getMutex()
{
    return mutex;
}

bool AverageCounter::getValues(double& first, double& second)
{
    if (values.size() < 2)
        return false;

    first = values.front();
    values.pop();

    second = values.front();
    values.pop();

    return true;
}

void AverageCounter::pushSum(double sum)
{
    values.push(sum);
    sumsDone++;
    if (sumsDone + 1 == initialSize)
    {
        auto res = values.front() / initialSize;
        average = res;
        ready = true;
    }
}

double AverageCounter::getResult()
{
    while(!ready)
        continue;

    return average;
}

void AverageCounter::count(AverageCounter& counter, ThreadPool& pool)
{
    std::vector<std::unique_ptr<SumJob>> tasks;
    const auto taskAmount = counter.initialSize / 2;
    tasks.reserve(taskAmount);
    for (size_t i = 0; i < taskAmount; ++i)
        tasks.emplace_back(std::make_unique<SumJob>(counter, pool));

    pool.push(tasks.begin(), tasks.end());
}

size_t AverageCounter::valuesSize() const
{
    return values.size();
}
