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

#include <float.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef GMIO_OS_WIN
#  include <windows.h>
#  define BENCHMARK_TIMER_WINDOWS
#else
#  include <time.h>
#  define BENCHMARK_TIMER_LIBC
#endif

#include "../../src/gmio_core/internal/c99_stdio_compat.h"
#include "../../src/gmio_core/internal/string.h"

/* Benchmark timers */

struct benchmark_timer
{
#ifdef BENCHMARK_TIMER_WINDOWS
    LARGE_INTEGER start_time;
    LARGE_INTEGER frequency;
#elif defined(BENCHMARK_TIMER_LIBC)
    clock_t start_tick;
#endif
};

static void benchmark_timer_start(struct benchmark_timer* timer)
{
#ifdef BENCHMARK_TIMER_WINDOWS
    QueryPerformanceFrequency(&timer->frequency);
    QueryPerformanceCounter(&timer->start_time);
#elif defined(BENCHMARK_TIMER_LIBC)
    timer->start_tick = clock();
#endif
}

static gmio_time_ms_t benchmark_timer_elapsed_ms(const struct benchmark_timer* timer)
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
    const clock_t elapsed_ticks = clock() - timer->start_tick;
    const double elapsed_ms = (elapsed_ticks * 1000) / ((double)CLOCKS_PER_SEC);
    return (gmio_time_ms_t)elapsed_ms;
#endif
}

/* Wraps around formatted printing functions */

/*! Wrap around snprintf() to be used with gprintf_func_exec_time() */
static void snprintf_wrap(void* cookie, const char* fmt, ...)
{
    struct gmio_string* str = (struct gmio_string*)cookie;
    va_list args;
    va_start(args, fmt);
    gmio_vsnprintf(str->ptr, str->max_len, fmt, args);
    va_end(args);
}

/*! Wrap around fprintf() to be used with gprintf_func_exec_time() */
static void fprintf_wrap(void* cookie, const char* fmt, ...)
{
    FILE* file = (FILE*)cookie;
    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args);
    va_end(args);
}

/*! Typedef on pointer to printf-wrap functions(eg. snprintf_wrap()) */
typedef void (*func_gprintf_t)(void*, const char*, ...);


/* Utilities */

/*! Calls printf(str) \p n times */
static void print_string_n(const char* str, size_t n)
{
    size_t i; /* for-loop index*/
    for (i = 0; i < n; ++i)
        fputs(str, stdout);
}

/*! Safe wrapper around strlen() for NULL strings */
GMIO_INLINE size_t safe_strlen(const char* str)
{
    return str != NULL ? strlen(str) : 0;
}

/*! Returns the maximum of two size_t values */
GMIO_INLINE size_t size_t_max(size_t lhs, size_t rhs)
{
    return lhs > rhs ? lhs : rhs;
}

/*! String representation of the unit used for execution time */
static const char unit_time_str[] = "ms";

/*! String representation of some data not available */
static const char n_a[] = "N/A";

/*! Generic formatted print of a string */
static void gprintf_func_string(
        /* Annex data for func_gprintf (ex: char* for sprintf()) */
        void* cookie,
        /* Function ptr on a printf wrap (ex: snprintf_wrap()) */
        func_gprintf_t func_gprintf,
        /* Width of the print column, if any (can be == 0) */
        size_t width_column,
        /* String to be printed */
        const char* str)
{
    if (width_column > 0)
        func_gprintf(cookie, "%-*s", width_column, str);
    else
        func_gprintf(cookie, "%s", str);
}

/*! Generic formatted print of some execution time */
static void gprintf_func_exec_time(
        void* cookie,
        func_gprintf_t func_gprintf,
        size_t width_column,
        gmio_time_ms_t time,
        bool has_time)
{
    if (has_time) {
        char str_time[128] = {0};
        /* TODO: %ull is not accepted by mingw, find a fix(maybe %ul64) */
        gmio_snprintf(str_time,
                      sizeof(str_time),
                      "%u%s",
                      (unsigned)time,
                      unit_time_str);
        gprintf_func_string(cookie, func_gprintf, width_column, str_time);
    }
    else {
        gprintf_func_string(cookie, func_gprintf, width_column, n_a);
    }
}

/*! Generic formatted print of a ratio */
static void gprintf_func_exec_ratio(
        void* cookie,
        func_gprintf_t func_gprintf,
        size_t width_column,
        float ratio)
{
    if (!(ratio < 0)) { /* Valid ratio */
        char str_ratio[128] = {0};
        gmio_snprintf(str_ratio, sizeof(str_ratio), "%.2f", ratio);
        gprintf_func_string(cookie, func_gprintf, width_column, str_ratio);
    }
    else {
        gprintf_func_string(cookie, func_gprintf, width_column, n_a);
    }
}

/*! Helper for printf() around gprintf_func_exec_time() */
static void printf_func_exec_time(
        size_t width_column,
        gmio_time_ms_t time_ms,
        bool has_time)
{
    gprintf_func_exec_time(
                stdout, &fprintf_wrap, width_column, time_ms, has_time);
}

/*! Returns the strlen of the longest tag string */
static size_t find_maxlen_cmp_result_tag(struct benchmark_cmp_result_array res_array)
{
    size_t max_len = 0;
    size_t i;
    for (i = 0; i < res_array.count; ++i) {
        const size_t len = safe_strlen(res_array.ptr[i].tag);
        max_len = size_t_max(len, max_len);
    }
    return max_len;
}

/*! Writes in output args the func1 execution informations */
static void select_cmp_result_func1_exec_infos(
        const struct benchmark_cmp_result* cmp,
        gmio_time_ms_t* time,
        bool* has_time)
{
    *time = cmp->func1_exec_time_ms;
    *has_time = cmp->has_func1_exec_time;
}

/*! Writes in output args the func2 execution informations */
static void select_cmp_result_func2_exec_infos(
        const struct benchmark_cmp_result* cmp,
        gmio_time_ms_t* time,
        bool* has_time)
{
    *time = cmp->func2_exec_time_ms;
    *has_time = cmp->has_func2_exec_time;
}

/*! Typedef on pointer to functions like select_cmp_result_funcX_exec_infos() */
typedef void (*func_select_cmp_result_func_exec_infos_t)(
        const struct benchmark_cmp_result*, gmio_time_ms_t*, bool*);

/*! Returns the strlen of the longest execution time string */
static size_t find_maxlen_cmp_result_func_exec_time(
        struct benchmark_cmp_result_array res_array,
        func_select_cmp_result_func_exec_infos_t func_select_exec_infos)
{
    char strbuff[1024] = {0};
    struct gmio_string str = gmio_string(strbuff, 0, sizeof(strbuff));
    size_t max_len = 0;
    size_t i;
    for (i = 0; i < res_array.count; ++i) {
        gmio_time_ms_t time = 0;
        bool has_time = false;
        func_select_exec_infos(&res_array.ptr[i], &time, &has_time);
        gprintf_func_exec_time(&str, &snprintf_wrap, 0, time, has_time);
        max_len = size_t_max(safe_strlen(strbuff), max_len);
    }
    return max_len;
}

/*! Returns the strlen of the longest func2/func1 ratio string */
static size_t find_maxlen_cmp_result_ratio(
        struct benchmark_cmp_result_array res_array)
{
    char strbuff[1024] = {0};
    struct gmio_string str = gmio_string(strbuff, 0, sizeof(strbuff));
    size_t max_len = 0;
    size_t i;
    for (i = 0; i < res_array.count; ++i) {
        const float ratio = res_array.ptr[i].func2_func1_ratio;
        gprintf_func_exec_ratio(&str, &snprintf_wrap, 0, ratio);
        max_len = size_t_max(safe_strlen(strbuff), max_len);
    }
    return max_len;
}

static void update_benchmark_cmp_result_ratio(
        struct benchmark_cmp_result* result)
{
    if (result->has_func1_exec_time && result->has_func2_exec_time) {
        if (result->func2_exec_time_ms > 0) {
            result->func2_func1_ratio =
                    result->func2_exec_time_ms
                    / (float)result->func1_exec_time_ms;
        }
        else {
            result->func2_func1_ratio = FLT_MAX;
        }
    }
    else {
        result->func2_func1_ratio = -1.f; /* Non-valid ratio */
    }
}

/* Implementation */

struct benchmark_cmp_result benchmark_cmp(struct benchmark_cmp_arg arg)
{
    struct benchmark_cmp_result result = {0};
    result.tag = arg.tag;

    if (arg.func1 != NULL) {
        struct benchmark_timer timer = {0};
        benchmark_timer_start(&timer);
        (*arg.func1)(arg.func1_arg);
        result.func1_exec_time_ms = benchmark_timer_elapsed_ms(&timer);
        result.has_func1_exec_time = true;
    }
    if (arg.func2 != NULL) {
        struct benchmark_timer timer = {0};
        benchmark_timer_start(&timer);
        (*arg.func2)(arg.func2_arg);
        result.func2_exec_time_ms = benchmark_timer_elapsed_ms(&timer);
        result.has_func2_exec_time = true;
    }
    update_benchmark_cmp_result_ratio(&result);

    return result;
}

void benchmark_cmp_batch(
        size_t run_count,
        const struct benchmark_cmp_arg *arg_array,
        struct benchmark_cmp_result *result_array,
        void (*func_init)(),
        void (*func_cleanup)())
{
    size_t run; /* for-loop index */
    size_t array_size = 0;
    while (arg_array[array_size].tag != NULL)
        ++array_size;

    for (run = 0; run < run_count; ++run) {
        size_t i; /* for-loop index */
        /* Init */
        if (func_init)
            (*func_init)();

        for (i = 0; i < array_size; ++i) {
            const struct benchmark_cmp_result ires = benchmark_cmp(arg_array[i]);
            struct benchmark_cmp_result* fres = &result_array[i];
            if (run != 0) {
                if (fres->func1_exec_time_ms > ires.func1_exec_time_ms)
                    fres->func1_exec_time_ms = ires.func1_exec_time_ms;
                if (fres->func2_exec_time_ms > ires.func2_exec_time_ms)
                    fres->func2_exec_time_ms = ires.func2_exec_time_ms;
                update_benchmark_cmp_result_ratio(fres);
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

void benchmark_print_results(
        enum benchmark_print_format format,
        struct benchmark_cmp_result_header header,
        struct benchmark_cmp_result_array result_array)
{
    if (format == BENCHMARK_PRINT_FORMAT_MARKDOWN) {
        const char* header_comp1 =
                header.component_1 != NULL ?  header.component_1 : "";
        const char* header_comp2 =
                header.component_2 != NULL ?  header.component_2 : "";
        const char header_ratio[] = "ratio";
        const size_t width_tag_col =
                find_maxlen_cmp_result_tag(result_array);
        const size_t width_func1_col =
                size_t_max(
                    find_maxlen_cmp_result_func_exec_time(
                        result_array, &select_cmp_result_func1_exec_infos),
                    safe_strlen(header_comp1));
        const size_t width_func2_col =
                size_t_max(
                    find_maxlen_cmp_result_func_exec_time(
                        result_array, &select_cmp_result_func2_exec_infos),
                    safe_strlen(header_comp2));
        const size_t width_ratio_col =
                size_t_max(
                    find_maxlen_cmp_result_ratio(result_array),
                    safe_strlen(header_ratio));
        size_t i; /* for-loop index*/

        /* Print table header */
        printf("%*s | ", (int)width_tag_col, "");
        printf("%-*s | ", (int)width_func1_col, header_comp1);
        printf("%-*s | ", (int)width_func2_col, header_comp2);
        printf("%-*s\n", (int)width_ratio_col, header_ratio);

        /* Print separation between header and results */
        print_string_n("-", width_tag_col + 1);
        printf("|");
        print_string_n("-", width_func1_col + 2);
        printf("|");
        print_string_n("-", width_func2_col + 2);
        printf("|");
        print_string_n("-", width_ratio_col + 2);
        printf("\n");

        /* Print benchmark result lines */
        for (i = 0; i < result_array.count; ++i) {
            const struct benchmark_cmp_result result = result_array.ptr[i];
            printf("%-*s | ", (int)width_tag_col, result.tag);
            printf_func_exec_time(
                        width_func1_col,
                        result.func1_exec_time_ms,
                        result.has_func1_exec_time);
            printf(" | ");
            printf_func_exec_time(
                        width_func2_col,
                        result.func2_exec_time_ms,
                        result.has_func2_exec_time);
            printf(" | ");
            gprintf_func_exec_ratio(
                        stdout, fprintf_wrap,
                        width_ratio_col, result.func2_func1_ratio);
            printf("\n");
        }
    }
}
