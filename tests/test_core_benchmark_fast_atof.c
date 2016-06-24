/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include "utest_assert.h"

#include "../benchmarks/commons/benchmark_tools.h"
#include "../src/gmio_core/internal/c99_stdio_compat.h"
#include "../src/gmio_core/internal/fast_atof.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static float __tc__float_array[1024] = {0};

static void __tc__fill_float_array()
{
    const float fmax = 1e6;
    size_t i;

    srand((unsigned)time(NULL));

    for (i = 0; i < GMIO_ARRAY_SIZE(__tc__float_array); ++i) {
        const double dsign = (i % 2) == 0 ? 1. : -1.;
        const double drand = (double)rand();
        const double drand_max = (double)RAND_MAX;
        const double dmax = (double)fmax;
        __tc__float_array[i] = (float)(dsign * (drand / drand_max) * dmax);
    }
}

static void __tc__run_atof(float (*func_atof)(const char*))
{
    char strbuff[512] = {0};
    size_t iter;
    for (iter = 0; iter < 250; ++iter) {
        size_t i;
        for (i = 0; i < GMIO_ARRAY_SIZE(__tc__float_array); ++i) {
            const float f = __tc__float_array[i];
            float fres = 0.f;
            gmio_snprintf(strbuff, sizeof(strbuff), "%f", f);
            fres = func_atof(strbuff);
            gmio_snprintf(strbuff, sizeof(strbuff), "%E", f);
            fres = func_atof(strbuff);
        }
    }
}

static float __tc__float_strtod(const char* str)
{
    return (float)strtod(str, NULL);
}

static void __tc__benchmark_fast_atof(const void* arg)
{
    GMIO_UNUSED(arg);
    __tc__run_atof(&fast_atof);
}

static void __tc__benchmark_strtod(const void* arg)
{
    GMIO_UNUSED(arg);
    __tc__run_atof(&__tc__float_strtod);
}

static const char* test_internal__benchmark_gmio_fast_atof()
{
    struct benchmark_cmp_arg bmk_arg[] = {
        { "str->float",
                &__tc__benchmark_fast_atof, NULL,
                &__tc__benchmark_strtod, NULL },
        {0}
    };
    struct benchmark_cmp_result bmk_res[] = { {0}, {0} };
    const struct benchmark_cmp_result_header header = { "fast_atof", "strtod" };
    struct benchmark_cmp_result_array bmk_res_array = {0};

    __tc__fill_float_array();
    benchmark_cmp_batch(2, bmk_arg, bmk_res, NULL, NULL);
    bmk_res_array.ptr = bmk_res;
    bmk_res_array.count = GMIO_ARRAY_SIZE(bmk_res) - 1;
    puts("\n");
    benchmark_print_results(
                BENCHMARK_PRINT_FORMAT_MARKDOWN,
                header,
                bmk_res_array);

#ifndef GMIO_DEBUG_BUILD /* Check only for release builds */
    UTEST_ASSERT((1.05*bmk_res[0].func1_exec_time_ms) < bmk_res[0].func2_exec_time_ms);
#endif

    return NULL;
}
