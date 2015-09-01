/****************************************************************************
** gmio benchmarks
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software provides performance benchmarks for the gmio library
** (https://github.com/fougue/gmio)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include "benchmark_tools.h"

#include <stdio.h>

#ifdef WIN32
#  include <windows.h>
#  define BENCHMARK_TIMER_WINDOWS
#else
#  define BENCHMARK_TIMER_LIBC
#endif

typedef struct benchmark_timer
{
#ifdef BENCHMARK_TIMER_WINDOWS
    LARGE_INTEGER start_time;
    LARGE_INTEGER frequency;
#elif defined(BENCHMARK_TIMER_LIBC)
    clock_t start_tick;
#endif
} benchmark_timer_t;

void benchmark_timer_start(benchmark_timer_t* timer)
{
#ifdef BENCHMARK_TIMER_WINDOWS
    QueryPerformanceFrequency(&timer->frequency);
    QueryPerformanceCounter(&timer->start_time);
#elif defined(BENCHMARK_TIMER_LIBC)
    timer->start_tick = clock();
#endif
}

int64_t benchmark_timer_elapsed_ms(const benchmark_timer_t* timer)
{
#ifdef BENCHMARK_TIMER_WINDOWS
    LARGE_INTEGER end_time = { 0 };
    LARGE_INTEGER elapsed = { 0 };
    QueryPerformanceCounter(&end_time);
    elapsed.QuadPart = end_time.QuadPart - timer->start_time.QuadPart;

    /*
     We now have the elapsed number of ticks, along with the
     number of ticks-per-second. We use these values
     to convert to the number of elapsed microseconds.
     To guard against loss-of-precision, we convert
     to milliseconds *before* dividing by ticks-per-second.
     */

    elapsed.QuadPart *= 1000;
    elapsed.QuadPart /= timer->frequency.QuadPart;
    return elapsed.QuadPart;
#elif defined(BENCHMARK_TIMER_LIBC)
    /* For seconds:
     * return (float)((clock() - start_tick) / (float)CLOCKS_PER_SEC); */
    return clock() - timer->start_tick;
#endif
}

void benchmark_list(
        benchmark_file_func_t func, const char* title, int argc, char **argv)
{
    benchmark_timer_t timer = { 0 };
    int iarg;

    if (func == NULL)
        return;

    benchmark_timer_start(&timer);

    printf("Bench %s ...\n", title);
    for (iarg = 0; iarg < argc; ++iarg) {
        printf("  File %s ...\n", argv[iarg]);
        (*func)(argv[iarg]);
    }
    /*printf("  exec time: %.2fs\n\n", elapsed_secs(start_tick));*/
    printf("  exec time: %lldms\n\n", benchmark_timer_elapsed_ms(&timer));
}

void benchmark_forward_list(benchmark_file_func_t func, int argc, char **argv)
{
    int i = 0;
    while (i < argc) {
        func(argv[i]);
        ++i;
    }
}

void benchmark(
        benchmark_file_func_t func, const char *title, const char* filepath)
{
    benchmark_list(func, title, 1, (char**)&filepath);
}
