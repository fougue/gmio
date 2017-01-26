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
