#include "average_counter.h"
#include "sum_job.h"
#include "thread_pool.h"

#include <vector>
#include <numeric>
#include <iostream>


double CountAverageUsingAccumulate(std::vector<double>& values)
{
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double average = sum / values.size();

    std::cout << "Average using std::accumulate: " << average << std::endl;

    return average;
}

double CountAverageUsingThreads(std::vector<double>& values)
{
    ThreadPool pool;

    AverageCounter counter(values);
    AverageCounter::count(counter, pool);

    auto average = counter.getResult();

    std::cout << "Average using threads: " << average << std::endl;

    return average;
}

int main() try
{
    std::vector<double> values(10000);
    std::iota(values.begin(), values.end(), 0);

    auto accumulate = CountAverageUsingAccumulate(values);
    auto threads = CountAverageUsingThreads(values);

    assert(accumulate == threads);

    std::cout << "Done." << std::endl;

    return 0;
}
catch(const std::exception& e)
{
    std::cerr << ": " << e.what() << std::endl;
    return -1;
}
catch(...)
{
    std::cerr << ": Unknown exception" << std::endl;
    return -1;
}
