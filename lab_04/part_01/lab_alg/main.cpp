#include <iostream>
#include <thread>
#include <sys/types.h>
#include <unistd.h>

#include "conveyor.h"
#define N 100000

int main()
{
    double a[N];
    for (size_t i = 0; i < N; i++)
        a[i] = i;

    double b[N];

    size_t n_tasks;
    std::cout << "Number of tasks: ";
    std::cin >> n_tasks;

    setbuf(stdout, NULL);
    Conveyor *conveyor_obj = new Conveyor();

    //
    system_clock::time_point begin = system_clock::now();
    system_clock::duration tp_begin = begin.time_since_epoch();
    tp_begin -= duration_cast<seconds>(tp_begin);
    time_t tt_begin = system_clock::to_time_t(begin);
    tm t_begin = *gmtime(&tt_begin);

    conveyor_obj->run_parallel(n_tasks, N, a, b);

    system_clock::time_point end = system_clock::now();
    system_clock::duration tp_end = end.time_since_epoch();
    tp_end -= duration_cast<seconds>(tp_end);
    time_t tt_end = system_clock::to_time_t(end);
    tm t_end = *gmtime(&tt_end);

    size_t h_dur = t_end.tm_hour - t_begin.tm_hour;
    size_t m_dur = t_end.tm_min - t_begin.tm_min;
    size_t s_dur = t_end.tm_sec - t_begin.tm_sec;
    system_clock::duration ms_dur;
    if (tp_end >= tp_begin)
        ms_dur = tp_end - tp_begin;
    else
        ms_dur = milliseconds(1000) - (tp_begin - tp_end);
    size_t int_ms_dur = static_cast<unsigned>(ms_dur / milliseconds(1));
    //

//    std::printf("\n\nArray before standartization:\n");
//    for (size_t i = 0; i < N; i++)
//    {
//        std::printf("%3.1f  ", (double) i);
//        if ((i + 1) % 10 == 0)
//            std::printf("\n");
//    }
//    std::printf("\n\n");
//    std::printf("\n\nArray after standartization:\n");
//    for (size_t i = 0; i < N; i++)
//    {
//        std::printf("%3.1f  ", conveyor_obj->tasks.front()->new_arr[i]);
//        if ((i + 1) % 10 == 0)
//            std::printf("\n");
//    }
//    std::printf("\n\n");

    conveyor_obj->make_stats(n_tasks);
    conveyor_obj->clean_tasks(n_tasks);

    //
    system_clock::time_point begin_l = system_clock::now();
    system_clock::duration tp_begin_l = begin_l.time_since_epoch();
    tp_begin_l -= duration_cast<seconds>(tp_begin_l);
    time_t tt_begin_l = system_clock::to_time_t(begin_l);
    tm t_begin_l = *gmtime(&tt_begin_l);

    conveyor_obj->run_linear(n_tasks, N, a, b);

    system_clock::time_point end_l = system_clock::now();
    system_clock::duration tp_end_l = end_l.time_since_epoch();
    tp_end_l -= duration_cast<seconds>(tp_end_l);
    time_t tt_end_l = system_clock::to_time_t(end_l);
    tm t_end_l = *gmtime(&tt_end_l);

    size_t h_dur_l = t_end_l.tm_hour - t_begin_l.tm_hour;
    size_t m_dur_l = t_end_l.tm_min - t_begin_l.tm_min;
    size_t s_dur_l = t_end_l.tm_sec - t_begin_l.tm_sec;
    system_clock::duration ms_dur_l;
    if (tp_end_l >= tp_begin_l)
        ms_dur_l = tp_end_l - tp_begin_l;
    else
        ms_dur_l = milliseconds(1000) - (tp_begin_l - tp_end_l);
    size_t int_ms_dur_l = static_cast<unsigned>(ms_dur_l / milliseconds(1));
    //

    //conveyor_obj->make_stats(n_tasks);
    conveyor_obj->clean_tasks(n_tasks);

    //
    size_t ms_total_par = int_ms_dur + 1000 * (s_dur + 60 * (m_dur + 60 * h_dur));
    size_t ms_total_lin = int_ms_dur_l + 1000 * (s_dur_l + 60 * (m_dur_l + 60 * h_dur_l));
    std::printf("Duration parallel %02llu:%02llu:%02llu.%3llu   (%3llu ms total)\n",
                h_dur, m_dur, s_dur, int_ms_dur, ms_total_par
    );

    std::printf("Duration linear   %02llu:%02llu:%02llu.%3llu   (%3llu ms total)\n",
                h_dur_l, m_dur_l, s_dur_l, int_ms_dur_l, ms_total_lin
    );

    if (ms_total_lin >= ms_total_par)
    {
        std::printf("Linear realization is %3llu ms (%.3f times) slower than parallel\n",
                    (ms_total_lin - ms_total_par), ((double)ms_total_lin / ms_total_par)
        );
    }
    else
    {
        std::printf("Parallel realization is %3llu ms (%.3f times) slower than linear\n",
                    (ms_total_par - ms_total_lin), ((double)ms_total_par / ms_total_lin)
        );
    }

    //time_cmp
    std::FILE *f = std::fopen("times.txt", "a+");
    int n_repeats = 7;
    for (n_tasks = 1; n_tasks <= 25; n_tasks *= 5)
    {
        size_t ms_total_par = 0;
        size_t ms_total_lin = 0;
        for (int repeat = 0; repeat < n_repeats; repeat++)
        {
            //
            system_clock::time_point begin = system_clock::now();
            system_clock::duration tp_begin = begin.time_since_epoch();
            tp_begin -= duration_cast<seconds>(tp_begin);
            time_t tt_begin = system_clock::to_time_t(begin);
            tm t_begin = *gmtime(&tt_begin);

            conveyor_obj->run_parallel(n_tasks, N, a, b);

            system_clock::time_point end = system_clock::now();
            system_clock::duration tp_end = end.time_since_epoch();
            tp_end -= duration_cast<seconds>(tp_end);
            time_t tt_end = system_clock::to_time_t(end);
            tm t_end = *gmtime(&tt_end);

            size_t h_dur = t_end.tm_hour - t_begin.tm_hour;
            size_t m_dur = t_end.tm_min - t_begin.tm_min;
            size_t s_dur = t_end.tm_sec - t_begin.tm_sec;
            system_clock::duration ms_dur;
            if (tp_end >= tp_begin)
                ms_dur = tp_end - tp_begin;
            else
                ms_dur = milliseconds(1000) - (tp_begin - tp_end);
            size_t int_ms_dur = static_cast<unsigned>(ms_dur / milliseconds(1));
            //

            conveyor_obj->clean_tasks(n_tasks);

            //
            system_clock::time_point begin_l = system_clock::now();
            system_clock::duration tp_begin_l = begin_l.time_since_epoch();
            tp_begin_l -= duration_cast<seconds>(tp_begin_l);
            time_t tt_begin_l = system_clock::to_time_t(begin_l);
            tm t_begin_l = *gmtime(&tt_begin_l);

            conveyor_obj->run_linear(n_tasks, N, a, b);

            system_clock::time_point end_l = system_clock::now();
            system_clock::duration tp_end_l = end_l.time_since_epoch();
            tp_end_l -= duration_cast<seconds>(tp_end_l);
            time_t tt_end_l = system_clock::to_time_t(end_l);
            tm t_end_l = *gmtime(&tt_end_l);

            size_t h_dur_l = t_end_l.tm_hour - t_begin_l.tm_hour;
            size_t m_dur_l = t_end_l.tm_min - t_begin_l.tm_min;
            size_t s_dur_l = t_end_l.tm_sec - t_begin_l.tm_sec;
            system_clock::duration ms_dur_l;
            if (tp_end_l >= tp_begin_l)
                ms_dur_l = tp_end_l - tp_begin_l;
            else
                ms_dur_l = milliseconds(1000) - (tp_begin_l - tp_end_l);
            size_t int_ms_dur_l = static_cast<unsigned>(ms_dur_l / milliseconds(1));

            conveyor_obj->clean_tasks(n_tasks);

            //
            ms_total_par += (int_ms_dur + 1000 * (s_dur + 60 * (m_dur + 60 * h_dur)));
            ms_total_lin += (int_ms_dur_l + 1000 * (s_dur_l + 60 * (m_dur_l + 60 * h_dur_l)));

            std::printf("\n\n\nn_tasks=%llu, n_repeat=%d\n\n\n\n\n", n_tasks, repeat);
        }
        ms_total_par /= n_repeats;
        ms_total_lin /= n_repeats;

        std::fprintf(f, "%llu   %llu    %llu\n", n_tasks, ms_total_lin, ms_total_par);
        std::printf("\n\n\n\n\n\n\n\n");
        std::fprintf(stdout, "%llu   %llu    %llu", n_tasks, ms_total_lin, ms_total_par);
        std::printf("\n\n\n\n\n\n\n\n");
    }
    for (n_tasks = 50; n_tasks <= 250; n_tasks += 50)
    {
        size_t ms_total_par = 0;
        size_t ms_total_lin = 0;
        for (int repeat = 0; repeat < n_repeats; repeat++)
        {
            //
            system_clock::time_point begin = system_clock::now();
            system_clock::duration tp_begin = begin.time_since_epoch();
            tp_begin -= duration_cast<seconds>(tp_begin);
            time_t tt_begin = system_clock::to_time_t(begin);
            tm t_begin = *gmtime(&tt_begin);

            conveyor_obj->run_parallel(n_tasks, N, a, b);

            system_clock::time_point end = system_clock::now();
            system_clock::duration tp_end = end.time_since_epoch();
            tp_end -= duration_cast<seconds>(tp_end);
            time_t tt_end = system_clock::to_time_t(end);
            tm t_end = *gmtime(&tt_end);

            size_t h_dur = t_end.tm_hour - t_begin.tm_hour;
            size_t m_dur = t_end.tm_min - t_begin.tm_min;
            size_t s_dur = t_end.tm_sec - t_begin.tm_sec;
            system_clock::duration ms_dur;
            if (tp_end >= tp_begin)
                ms_dur = tp_end - tp_begin;
            else
                ms_dur = milliseconds(1000) - (tp_begin - tp_end);
            size_t int_ms_dur = static_cast<unsigned>(ms_dur / milliseconds(1));
            //

            conveyor_obj->clean_tasks(n_tasks);

            //
            system_clock::time_point begin_l = system_clock::now();
            system_clock::duration tp_begin_l = begin_l.time_since_epoch();
            tp_begin_l -= duration_cast<seconds>(tp_begin_l);
            time_t tt_begin_l = system_clock::to_time_t(begin_l);
            tm t_begin_l = *gmtime(&tt_begin_l);

            conveyor_obj->run_linear(n_tasks, N, a, b);

            system_clock::time_point end_l = system_clock::now();
            system_clock::duration tp_end_l = end_l.time_since_epoch();
            tp_end_l -= duration_cast<seconds>(tp_end_l);
            time_t tt_end_l = system_clock::to_time_t(end_l);
            tm t_end_l = *gmtime(&tt_end_l);

            size_t h_dur_l = t_end_l.tm_hour - t_begin_l.tm_hour;
            size_t m_dur_l = t_end_l.tm_min - t_begin_l.tm_min;
            size_t s_dur_l = t_end_l.tm_sec - t_begin_l.tm_sec;
            system_clock::duration ms_dur_l;
            if (tp_end_l >= tp_begin_l)
                ms_dur_l = tp_end_l - tp_begin_l;
            else
                ms_dur_l = milliseconds(1000) - (tp_begin_l - tp_end_l);
            size_t int_ms_dur_l = static_cast<unsigned>(ms_dur_l / milliseconds(1));

            conveyor_obj->clean_tasks(n_tasks);

            //
            ms_total_par += (int_ms_dur + 1000 * (s_dur + 60 * (m_dur + 60 * h_dur)));
            ms_total_lin += (int_ms_dur_l + 1000 * (s_dur_l + 60 * (m_dur_l + 60 * h_dur_l)));

            std::printf("\n\n\nn_tasks=%llu, n_repeat=%d\n\n\n\n\n", n_tasks, repeat);
        }
        ms_total_par /= n_repeats;
        ms_total_lin /= n_repeats;

        std::fprintf(f, "%llu   %llu    %llu\n", n_tasks, ms_total_lin, ms_total_par);
        std::printf("\n\n\n\n\n\n\n\n");
        std::fprintf(stdout, "%llu   %llu    %llu", n_tasks, ms_total_lin, ms_total_par);
        std::printf("\n\n\n\n\n\n\n\n");
    }
    std::fclose(f);

    delete conveyor_obj;

    
    fclose(f);
    return 0;
}
