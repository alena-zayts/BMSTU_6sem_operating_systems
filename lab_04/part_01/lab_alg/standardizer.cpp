#include "standardizer.h"

Mean::Mean(size_t n, double *arr)
{
    double sum = 0;
    for (size_t i = 0; i < n; i++)
    {
        sum += arr[i];
    }

    this->mean = sum / n;
}

StdDev::StdDev(size_t n, double *arr, double mean)
{    
    double sum = 0;
    for (size_t i = 0; i < n; i++)
    {
        sum += (arr[i] - mean) * (arr[i] - mean);
    }

    this->std_dev = sqrt(sum / n);
}

Transformer::Transformer(size_t n, double *arr, double mean, double std_dev, double *new_arr)
{
    for (size_t i = 0; i < n; i++)
    {
        new_arr[i] = (arr[i] - mean) / std_dev;
    }
}

Standardizer::Standardizer(size_t n, double *arr, double *new_arr)
{
    this->n = n;
    this->arr = arr;
    this->new_arr = new_arr;
}

void Standardizer::find_mean(size_t task_num)
{
    Logger::log_current_event(task_num, "P1 | Start", 1);
    this->meanC = std::unique_ptr<Mean>(new Mean(this->n, this->arr));
    Logger::log_current_event(task_num, "P1 | End  ", 1);
}

void Standardizer::find_std_dev(size_t task_num)
{
    Logger::log_current_event(task_num, "P2 | Start", 2);
    this->stdDevC = std::unique_ptr<StdDev>(new StdDev(this->n, this->arr, this->meanC->mean));
    Logger::log_current_event(task_num, "P2 | End  ", 2);
}

void Standardizer::transform(size_t task_num)
{
    Logger::log_current_event(task_num, "P3 | Start", 3);
    this->transformer = std::unique_ptr<Transformer>(new Transformer(this->n,
                                                                     this->arr,
                                                                     this->meanC->mean,
                                                                     this->stdDevC->std_dev,
                                                                     this->new_arr));
    Logger::log_current_event(task_num, "P3 | End  ", 3);
}
