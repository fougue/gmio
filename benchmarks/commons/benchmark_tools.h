/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#pragma once

#include "../../src/gmio_core/global.h"
#include <stddef.h>

GMIO_C_LINKAGE_BEGIN

#ifdef GMIO_HAVE_INT64_TYPE
typedef uint64_t gmio_time_ms_t;
#else
typedef size_t gmio_time_ms_t;
#endif

/*! Typedef on pointer to function to be benchmarked(execution time) */
typedef void (*benchmark_func_t)(const void*);

/* benchmark_cmp */

/*! Describes a comparison benchmark between two functions */
struct benchmark_cmp_arg
{
    /*! Brief description of the comparison(eg. "Write to file") */
    const char* tag;
    /*! Pointer to the 1st function */
    benchmark_func_t func1;
    /*! Argument passed to the 1st function on exec */
    const void* func1_arg;
    /*! Pointer to the 2nd function */
    benchmark_func_t func2;
    /*! Argument passed to the 2nd function on exec */
    const void* func2_arg;
};

/*! Holds the result of the exec time comparison between two functions */
struct benchmark_cmp_result
{
    /*! Identifies the comparison */
    const char* tag;
    /*! Execution time(in ms) of the 1st function */
    gmio_time_ms_t func1_exec_time_ms;
    /*! Is exec time of the 1st function valid ? */
    bool has_func1_exec_time;
    /*! Execution time(in ms) of the 2nd function */
    gmio_time_ms_t func2_exec_time_ms;
    /*! Is exec time of the 2nd function valid ? */
    bool has_func2_exec_time;
    float func2_func1_ratio;
};

/*! Runs func1 then func2 and measures the respective execution time */
struct benchmark_cmp_result benchmark_cmp(struct benchmark_cmp_arg arg);

/*! Runs a batch(array) of comparison benchmarks */
void benchmark_cmp_batch(
        size_t run_count,
        const struct benchmark_cmp_arg* arg_array,
        struct benchmark_cmp_result* result_array,
        void (*func_init)(),
        void (*func_cleanup)());


/* benchmark_print_results */

enum benchmark_print_format
{
    BENCHMARK_PRINT_FORMAT_MARKDOWN = 0
};

/*! Array of benchmark_cmp_result */
struct benchmark_cmp_result_array
{
    const struct benchmark_cmp_result* ptr;
    size_t count;
};

/*! Horizontal header labels for benchmark results(by column) */
struct benchmark_cmp_result_header
{
    const char* component_1;
    const char* component_2;
};

/*! Prints formatted benchmark results */
void benchmark_print_results(
        enum benchmark_print_format format,
        struct benchmark_cmp_result_header header,
        struct benchmark_cmp_result_array result_array);

GMIO_C_LINKAGE_END
