#ifndef __CONVEYOR_H__
#define __CONVEYOR_H__

#include <thread>
#include <queue>

#include "standardizer.h"
#include <mutex>

#define numThreads 3

class Conveyor
{
public:
    Conveyor() = default;
    ~Conveyor() = default;

    void run_parallel(size_t n_tasks, size_t n, double *arr, double *new_arr);
    void run_linear(size_t n_tasks, size_t n, double *arr, double *new_arr);

    void find_mean(size_t n_tasks);
    void find_std_dev(size_t n_tasks);
    void transform(size_t n_tasks);

    void make_stats(size_t n_tasks);
    void clean_tasks(size_t n_tasks);
    size_t time_dif(system_clock::time_point begin, system_clock::time_point end, char t);

    std::vector<std::shared_ptr<Standardizer>> tasks;

private:
    std::thread threads[numThreads];

    std::mutex m2;
    std::mutex m3;

    std::queue<std::shared_ptr<Standardizer>> q1;
    std::queue<std::shared_ptr<Standardizer>> q2;
    std::queue<std::shared_ptr<Standardizer>> q3;
};

#endif
