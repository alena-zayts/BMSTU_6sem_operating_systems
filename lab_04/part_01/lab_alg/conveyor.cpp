#include "conveyor.h"
#include <sys/types.h>
#include <unistd.h>

void Conveyor::run_parallel(size_t n_tasks, size_t n, double *arr, double *new_arr)
{
    for (size_t i = 0; i < n_tasks; i++)
    {
        std::shared_ptr<Standardizer> new_task(new Standardizer(n, arr, new_arr));
        q1.push(new_task);
        new_task->in1 = system_clock::now();
    }

    this->threads[0] = std::thread(&Conveyor::find_mean, this, n_tasks);
    this->threads[1] = std::thread(&Conveyor::find_std_dev, this, n_tasks);
    this->threads[2] = std::thread(&Conveyor::transform, this, n_tasks);

    FILE *f1 = std::fopen("test_open.txt", "w");
    for (int k = 0; k < 1000; k++)
    {
        std::printf("Still working with 3 additional threads, pid = %d\n", getpid());
        sleep(20);
    }
    
    for (size_t i = 0; i < numThreads; i++)
    {
        //if (this->threads[i].joinable())
            this->threads[i].join();
    }
}

void Conveyor::run_linear(size_t n_tasks, size_t n, double *arr, double *new_arr)
{
    for (size_t i = 0; i < n_tasks; i++)
    {
        std::shared_ptr<Standardizer> task(new Standardizer(n, arr, new_arr));

        task->find_mean(i + 1);
        task->find_std_dev(i + 1);
        task->transform(i + 1);

        tasks.push_back(task);
    }
}

//void Conveyor::run_linear(size_t n_tasks, size_t n, double *arr, double *new_arr)
//{
//    for (size_t i = 0; i < n_tasks; i++)
//    {
//        std::shared_ptr<Standardizer> new_task(new Standardizer(n, arr, new_arr));
//        q1.push(new_task);
//        new_task->in1 = system_clock::now();
//    }

//    for (size_t i = 0; i < n_tasks; i++)
//    {
//        std::shared_ptr<Standardizer> task = q1.front();
//        q1.pop();
//        task->out1 = system_clock::now();

//        task->find_mean(i + 1);

//        q2.push(task);
//        task->in2 = system_clock::now();


//        task = q2.front();
//        q2.pop();
//        task->out2 = system_clock::now();
//        task->find_std_dev(i + 1);
//        q3.push(task);
//        task->in3 = system_clock::now();


//        task = q3.front();
//        task->out3 =system_clock::now();
//        task->transform(i + 1);
//        tasks.push_back(task);
//        task->out_system = system_clock::now();
//        q3.pop();
//    }
//}

void Conveyor::find_mean(size_t n_tasks)
{
    size_t task_num = 0;

    while (task_num < n_tasks)
    {
        std::shared_ptr<Standardizer> task = q1.front();
        q1.pop();
        task->out1 = system_clock::now();

        task->find_mean(++task_num);

        m2.lock();
        q2.push(task);
        task->in2 = system_clock::now();
        m2.unlock();
    }
}

void Conveyor::find_std_dev(size_t n_tasks)
{
    size_t task_num = 0;

    while(task_num < n_tasks)
    {
        m2.lock();
        if (!this->q2.empty())
        {
            std::shared_ptr<Standardizer> task = q2.front();
            q2.pop();
            task->out2 = system_clock::now();
            m2.unlock();

            task->find_std_dev(++task_num);

            m3.lock();
            q3.push(task);
            task->in3 = system_clock::now();
            m3.unlock();
        }
        else
            m2.unlock();
    }
}

void Conveyor::transform(size_t n_tasks)
{
    size_t task_num = 0;

    while (task_num < n_tasks)
    {
        m3.lock();
        if (!this->q3.empty())
        {
            std::shared_ptr<Standardizer> task = q3.front();
            q3.pop();
            task->out3 = system_clock::now();
            m3.unlock();

            task->transform(++task_num);

            tasks.push_back(task);
            task->out_system = system_clock::now();

        }
        else
            m3.unlock();
    }
}

void Conveyor::clean_tasks(size_t n_tasks)
{
    for (size_t i = 0; i < n_tasks; i++)
    {
        tasks.pop_back();
    }
}

void Conveyor::make_stats(size_t n_tasks)
{
    std::printf("\n\n\n########################### STATISTICS (in ms) ###########################\n");
    size_t time_q1_min = SIZE_MAX;//this->time_dif(tasks[0]->in1, tasks[0]->out1, 'T');
    size_t time_q1_max = 0;//time_q1_min;
    size_t time_q1_mean = 0;

    size_t time_q2_min = SIZE_MAX;//this->time_dif(tasks[0]->in2, tasks[0]->out2, 'T');
    size_t time_q2_max = 0;//time_q2_min;
    size_t time_q2_mean = 0;

    size_t time_q3_min = SIZE_MAX;//this->time_dif(tasks[0]->in3, tasks[0]->out3, 'T');
    size_t time_q3_max = 0;//time_q3_min;
    size_t time_q3_mean = 0;

    size_t time_system_min = SIZE_MAX;//this->time_dif(tasks[0]->in1, tasks[0]->out_system, 'T');
    size_t time_system_max = 0;//time_system_min;
    size_t time_system_mean = 0;

    size_t time_queue_min = SIZE_MAX;//time_q1_min + time_q2_min + time_q3_min;
    size_t time_queue_max = 0;//time_queue_min;
    size_t time_queue_mean = 0;

    for (size_t i = 0; i < n_tasks; i++)
    {
        size_t time_q1_cur = this->time_dif(tasks[i]->in1, tasks[i]->out1, 'T');
        size_t time_q2_cur = this->time_dif(tasks[i]->in2, tasks[i]->out2, 'T');
        size_t time_q3_cur = this->time_dif(tasks[i]->in3, tasks[i]->out3, 'T');
        size_t time_system_cur = this->time_dif(tasks[i]->in1, tasks[i]->out_system, 'T');
        size_t time_queue_cur = time_q1_cur + time_q2_cur + time_q3_cur;


        if (time_q1_cur < time_q1_min)
            time_q1_min = time_q1_cur;
        if (time_q1_cur > time_q1_max)
            time_q1_max = time_q1_cur;
        time_q1_mean += time_q1_cur;

        if (time_q2_cur < time_q2_min)
            time_q2_min = time_q2_cur;
        if (time_q2_cur > time_q2_max)
            time_q2_max = time_q2_cur;
        time_q2_mean += time_q2_cur;

        if (time_q3_cur < time_q3_min)
            time_q3_min = time_q3_cur;
        if (time_q3_cur > time_q3_max)
            time_q3_max = time_q3_cur;
        time_q3_mean += time_q3_cur;

        if (time_system_cur < time_system_min)
            time_system_min = time_system_cur;
        if (time_system_cur > time_system_max)
            time_system_max = time_system_cur;
        time_system_mean += time_system_cur;


        if (time_queue_cur < time_queue_min)
            time_queue_min = time_queue_cur;
        if (time_queue_cur > time_queue_max)
            time_queue_max = time_queue_cur;
        time_queue_mean += time_queue_cur;

        if (time_system_cur < time_queue_cur)
            std::printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n%llu\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n",
                        i);
        if (tasks[i]->out_system < tasks[i]->out3)
            std::printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n%llu\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n",
                        i);
    }

    time_q1_mean /= n_tasks;
    time_q2_mean /= n_tasks;
    time_q3_mean /= n_tasks;
    time_system_mean /= n_tasks;
    time_queue_mean /= n_tasks;


    std::printf("Time in q1:           min=%10llu, max=%10llu, mean=%10llu\n",
                time_q1_min, time_q1_max, time_q1_mean);
    std::printf("Time in q2:           min=%10llu, max=%10llu, mean=%10llu\n",
                time_q2_min, time_q2_max, time_q2_mean);
    std::printf("Time in q3:           min=%10llu, max=%10llu, mean=%10llu\n",
                time_q3_min, time_q3_max, time_q3_mean);
    std::printf("Total time in queue:  min=%10llu, max=%10llu, mean=%10llu\n",
                time_queue_min, time_queue_max, time_queue_mean);
    std::printf("Total time in system: min=%10llu, max=%10llu, mean=%10llu\n",
                time_system_min, time_system_max, time_system_mean);

    std::printf("########################### ENS OF STATISTICS ###########################\n\n\n");
}

size_t Conveyor::time_dif(system_clock::time_point begin, system_clock::time_point end, char t)
{
    auto elapsed = duration_cast<milliseconds>(end - begin);
    system_clock::duration tp_begin = begin.time_since_epoch();
    tp_begin -= duration_cast<seconds>(tp_begin);
    time_t tt_begin = system_clock::to_time_t(begin);
    tm t_begin = *gmtime(&tt_begin);

    system_clock::duration tp_end = end.time_since_epoch();
    tp_end -= duration_cast<seconds>(tp_end);
    time_t tt_end = system_clock::to_time_t(end);
    tm t_end = *gmtime(&tt_end);

    size_t h_dur = t_end.tm_hour - t_begin.tm_hour;
    size_t m_dur = t_end.tm_min - t_begin.tm_min;
    size_t s_dur = t_end.tm_sec - t_begin.tm_sec;
    system_clock::duration ms;
    if (tp_end >= tp_begin)
        ms = tp_end - tp_begin;
    else
        ms = milliseconds(1000) - (tp_begin - tp_end);
    size_t ms_dur = static_cast<unsigned>(ms / milliseconds(1));
    size_t ms_total = ms_dur + 1000 * (s_dur + 60 * (m_dur + 60 * h_dur));

    switch (t) {
    case 'h':
        return h_dur;
        break;
    case 'm':
        return m_dur;
        break;
    case 's':
        return s_dur;
        break;
    case '1':
        return ms_dur;
        break;
    case 'T':
        return static_cast<unsigned>(elapsed / milliseconds(1));//ms_total;
        break;
    default:
        break;
    }
}


