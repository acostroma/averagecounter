#pragma once

#include "thread_pool.h"

class AverageCounter
{
public:
    AverageCounter(ThreadPool& pool, std::vector<double> values);
    double getResult();
private:
    double result;
};