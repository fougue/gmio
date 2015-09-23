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
#include <string.h>

#ifdef WIN32
#  include <windows.h>
#  define BENCHMARK_TIMER_WINDOWS
#else
#  include <time.h>
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

static void benchmark_timer_start(benchmark_timer_t* timer)
{
#ifdef BENCHMARK_TIMER_WINDOWS
    QueryPerformanceFrequency(&timer->frequency);
    QueryPerformanceCounter(&timer->start_time);
#elif defined(BENCHMARK_TIMER_LIBC)
    timer->start_tick = clock();
#endif
}

static int64_t benchmark_timer_elapsed_ms(const benchmark_timer_t* timer)
{
#ifdef BENCHMARK_TIMER_WINDOWS
    LARGE_INTEGER end_time = {0};
    LARGE_INTEGER elapsed = {0};
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

benchmark_cmp_result_t benchmark_cmp(benchmark_cmp_arg_t arg)
{
    benchmark_cmp_result_t result = {0};
    result.tag = arg.tag;

    if (arg.func1 != NULL) {
        benchmark_timer_t timer = {0};
        benchmark_timer_start(&timer);
        (*arg.func1)(arg.func1_filepath);
        result.func1_exec_time_ms = benchmark_timer_elapsed_ms(&timer);
        result.has_func1_exec_time = GMIO_TRUE;
    }
    if (arg.func2 != NULL) {
        benchmark_timer_t timer = {0};
        benchmark_timer_start(&timer);
        (*arg.func2)(arg.func2_filepath);
        result.func2_exec_time_ms = benchmark_timer_elapsed_ms(&timer);
        result.has_func2_exec_time = GMIO_TRUE;
    }

    return result;
}

static void printf_string_n(const char* str, size_t n)
{
    size_t i; /* for-loop index*/
    for (i = 0; i < n; ++i)
        printf(str);
}

static const int width_tag_col = 20;
static const int width_func_col = 10;
static const char* unit_time_str = "ms";

static void printf_func_exec_time(
        size_t func_exec_time_ms, gmio_bool_t has_func_exec_time)
{
    if (has_func_exec_time) {
        char str_exec_time[128] = {0};
        /* TODO: %ull is not accepted by mingw, find a fix(maybe %ul64) */
        sprintf(str_exec_time, "%u%s", func_exec_time_ms, unit_time_str);
        printf("%-*s", width_func_col, str_exec_time);
    }
    else {
        printf("%-*s", width_func_col, "N/A");
    }
}

void benchmark_print_results(
        benchmark_print_format_t format,
        benchmark_cmp_result_header_t header,
        benchmark_cmp_result_array_t result_array)
{
    if (format == BENCHMARK_PRINT_FORMAT_MARKDOWN) {
        size_t i; /* for-loop index*/

        /* Print table header */
        printf("%*s | ", width_tag_col, "");
        printf("%-*s | ", width_func_col, header.component_1);
        printf("%-*s\n", width_func_col, header.component_2);

        /* Print separation between header and results */
        printf_string_n("-", width_tag_col + 1);
        printf("|");
        printf_string_n("-", width_func_col + strlen(unit_time_str));
        printf("|");
        printf_string_n("-", width_func_col + strlen(unit_time_str));
        printf("\n");

        /* Print benchmark result lines */
        for (i = 0; i < result_array.count; ++i) {
            const benchmark_cmp_result_t result = result_array.ptr[i];
            printf("%-*s | ", width_tag_col, result.tag);
            printf_func_exec_time(
                        result.func1_exec_time_ms, result.has_func1_exec_time);
            printf(" | ");
            printf_func_exec_time(
                        result.func2_exec_time_ms, result.has_func2_exec_time);
            printf("\n");
        }
    }
}

void benchmark_cmp_batch(
        size_t run_count,
        const benchmark_cmp_arg_t *arg_array,
        benchmark_cmp_result_t *result_array,
        void (*func_init)(),
        void (*func_cleanup)())
{
    size_t run; /* for-loop index */
    size_t array_size = 0;
    while (arg_array[array_size].tag != NULL) {
        ++array_size;
    }

    for (run = 0; run < run_count; ++run) {
        size_t i; /* for-loop index */
        /* Init */
        if (func_init)
            (*func_init)();

        for (i = 0; i < array_size; ++i) {
            const benchmark_cmp_result_t ires = benchmark_cmp(arg_array[i]);
            benchmark_cmp_result_t* fres = &result_array[i];
            if  (run != 0) {
                if (fres->func1_exec_time_ms > ires.func1_exec_time_ms)
                    fres->func1_exec_time_ms = ires.func1_exec_time_ms;
                if (fres->func2_exec_time_ms > ires.func2_exec_time_ms)
                    fres->func2_exec_time_ms = ires.func2_exec_time_ms;
            }
            else {
                *fres = ires;
            }
        }

        /* Cleanup */
        if (func_cleanup)
            (*func_cleanup)();
    }
}
