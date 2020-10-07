#include "average_counter.h"

#include <iostream>


AverageCounter::AverageCounter(ThreadPool& pool, std::vector<double> values)
{
    double initialSize = values.size();

    int i = 0;

    while(true)
    {
        auto first = values.back();
        values.pop_back();
        auto second = values.back();
        values.pop_back();
        auto future = pool.push([first, second] { return (first + second); });
        values.push_back(future.get());
        i++;
        if ((initialSize - 1) == i)
        {
            double finalSum = values.front(); 
            auto future = pool.push([finalSum, initialSize] { return finalSum / initialSize; });
            result = future.get();
            break;
        }
    }
};

double AverageCounter::getResult()
{
    return result;
};