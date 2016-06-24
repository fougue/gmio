/****************************************************************************
** gmio benchmarks
** Copyright Fougue (24 Jun. 2016)
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

#ifndef BENCHMARK_TOOLS_H
#define BENCHMARK_TOOLS_H

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

#endif /* BENCHMARK_TOOLS_H */
