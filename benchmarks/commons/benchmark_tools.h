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

#ifndef BENCHMARK_TOOLS_H
#define BENCHMARK_TOOLS_H

#include <gmio_core/global.h>
#include <stddef.h>

GMIO_C_LINKAGE_BEGIN

typedef void (*benchmark_file_func_t)(const char*);

/* benchmark_cmp */

struct benchmark_cmp_arg
{
    const char* tag;
    benchmark_file_func_t func1;
    const char* func1_filepath;
    benchmark_file_func_t func2;
    const char* func2_filepath;
};
typedef struct benchmark_cmp_arg benchmark_cmp_arg_t;

struct benchmark_cmp_result
{
    const char* tag;
    size_t func1_exec_time_ms;
    gmio_bool_t has_func1_exec_time;
    size_t func2_exec_time_ms;
    gmio_bool_t has_func2_exec_time;
};
typedef struct benchmark_cmp_result benchmark_cmp_result_t;

benchmark_cmp_result_t benchmark_cmp(benchmark_cmp_arg_t arg);

void benchmark_cmp_batch(
        size_t run_count,
        const benchmark_cmp_arg_t* arg_array,
        benchmark_cmp_result_t* result_array,
        void (*func_init)(),
        void (*func_cleanup)());


/* benchmark_print_results */

enum benchmark_print_format
{
    BENCHMARK_PRINT_FORMAT_MARKDOWN = 0
};
typedef enum benchmark_print_format benchmark_print_format_t;

struct benchmark_cmp_result_array
{
    const benchmark_cmp_result_t* ptr;
    size_t count;
};
typedef struct benchmark_cmp_result_array benchmark_cmp_result_array_t;

struct benchmark_cmp_result_header
{
    const char* component_1;
    const char* component_2;
};
typedef struct benchmark_cmp_result_header benchmark_cmp_result_header_t;

void benchmark_print_results(
        benchmark_print_format_t format,
        benchmark_cmp_result_header_t header,
        benchmark_cmp_result_array_t result_array);

GMIO_C_LINKAGE_END

#endif /* BENCHMARK_TOOLS_H */
