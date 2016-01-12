/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
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
#include "../src/gmio_core/internal/fast_atof.h"

#include <stdio.h>
#include <stdlib.h>

static const float float_array[] = {
    1145816.0615274f,
    859213.75982427f,
    966810.0021625f,
    116009.39334184f,
    -71263.236445963f,
    1632844.0273334f,
    56876.761679015f,
    1670907.7036338f,
    1437992.5029064f,
    706598.88550015f,
    1912856.6587869f,
    -74873.963359219f,
    861707.30952253f,
    251769.8571327f,
    1699911.3574624f,
    187482.6645886f,
    635324.18922303f,
    825239.16816583f,
    923229.60096562f,
    1671951.443363f,
    1229446.7883787f,
    757413.85596684f,
    -14473.833429848f,
    1322835.7150326f,
    -33251.199514256f,
    1531123.3647313f,
    528501.5790856f,
    436167.23596871f,
    14397.062228246f,
    1454939.7571268f,
    1692972.0920012f,
    1780646.3084559f,
    989713.49335728f,
    1541628.0544557f,
    10818.210575179f,
    193697.32038756f,
    459958.03305877f,
    386664.66157262f,
    1424156.8543595f,
    1737569.8573131f,
    998766.07898565f,
    1498818.5395481f,
    1245966.4099132f,
    1455428.7210365f,
    197385.08439501f,
    879153.40041702f,
    -82192.77676297f,
    899980.71459121f,
    442951.18727859f,
    429733.95261435f,
    -91807.596567929f,
    242590.54984087f,
    38434.983528422f,
    1438265.2223754f,
    94147.7701972f,
    1883829.7321852f,
    692408.39047935f,
    1102390.5784368f,
    1195454.442173f,
    1016988.4061986f,
    329433.38976681f,
    34867.718878606f,
    1139500.6440764f,
    1105145.6145966f,
    857621.72278837f,
    385386.37328212f,
    99213.089933252f,
    1057370.9274909f,
    618583.85429652f,
    243663.42967547f,
    953197.6427665f,
    1965283.5770814f,
    1104619.5847935f,
    260492.339805f,
    234207.59892753f,
    1614202.0538515f,
    1189281.6673914f,
    1554272.0507617f,
    1025472.5255656f,
    1222277.3171599f,
    1453022.1682289f,
    1456530.4573889f,
    -85452.126751399f,
    12215.028755467f,
    1387416.2917898f,
    1526189.9514246f,
    1610725.9704781f,
    190781.76379706f,
    854656.48162861f,
    1351074.6843419f,
    551066.49000713f,
    52031.184291482f,
    1173924.6147098f,
    723591.50886703f,
    1147671.5720946f,
    918000.17292518f,
    1489562.4640815f,
    509914.95117076f,
    716195.46181345f,
    1981909.6994968f,
};

static void test_internal__run_atof(float (*func_atof)(const char*))
{
    size_t iter;
    for (iter = 0; iter < 1000; ++iter) {
        size_t i;
        for (i = 0; i < GMIO_ARRAY_SIZE(float_array); ++i) {
            char strbuff[512] = {0};
            const float f = float_array[i];
            float fres = 0.f;
            sprintf(strbuff, "%f", f);
            fres = func_atof(strbuff);
            sprintf(strbuff, "%E", f);
            fres = func_atof(strbuff);
        }
    }
}

static float float_strtod(const char* str)
{
    return (float)strtod(str, NULL);
}

static void benchmark_fast_atof(const char* dummy)
{
    GMIO_UNUSED(dummy);
    test_internal__run_atof(&fast_atof);
}

static void benchmark_strtod(const char* dummy)
{
    GMIO_UNUSED(dummy);
    test_internal__run_atof(&float_strtod);
}

const char* test_internal__benchmark_gmio_fast_atof()
{
    struct benchmark_cmp_arg bmk_arg[] = {
        { "str->float", &benchmark_fast_atof, NULL, &benchmark_strtod, NULL },
        {0}
    };
    struct benchmark_cmp_result bmk_res = {0};
    const struct benchmark_cmp_result_header header = { "fast_atof", "strtod" };
    struct benchmark_cmp_result_array bmk_res_array = {0};

    benchmark_cmp_batch(2, bmk_arg, &bmk_res, NULL, NULL);
    bmk_res_array.ptr = &bmk_res;
    bmk_res_array.count = 1;
    puts("\n");
    benchmark_print_results(
                BENCHMARK_PRINT_FORMAT_MARKDOWN,
                header,
                bmk_res_array);

    UTEST_ASSERT((1.2*bmk_res.func1_exec_time_ms) < bmk_res.func2_exec_time_ms);

    return NULL;
}
