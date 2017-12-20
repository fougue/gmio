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
#include "../src/gmio_core/internal/fast_atof.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>

namespace gmio {

namespace {

float TestCore_floatArray[1024] = {};

void TestCore_fillFloatArray()
{
    const float fmax = 1e6;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    size_t i = 0;
    for (float& f : TestCore_floatArray) {
        const double dsign = (i % 2) == 0 ? 1. : -1.;
        const double drand = static_cast<double>(std::rand());
        const double drand_max = static_cast<double>(RAND_MAX);
        const double dmax = static_cast<double>(fmax);
        f = static_cast<float>(dsign * (drand / drand_max) * dmax);
        ++i;
    }
}

void TestCore_runFuncStr2Float(const std::function<float (const char*)>& func_atof)
{
    char strbuff[512] = {};
    for (size_t iter = 0; iter < 250; ++iter) {
        for (const float f : TestCore_floatArray) {
            float fres = 0.f;
            std::snprintf(strbuff, sizeof(strbuff), "%f", f);
            fres = func_atof(strbuff);
            std::snprintf(strbuff, sizeof(strbuff), "%E", f);
            fres = func_atof(strbuff);
            GMIO_UNUSED(fres);
        }
    }
}

static void TestCore_run_fast_atof() {
    TestCore_runFuncStr2Float(&fast_atof);
}

void TestCore_run_strtof() {
    TestCore_runFuncStr2Float(
                [](const char* str) { return std::strtof(str, nullptr); });
}

} // namespace

static const char* TestCoreInternal_benchmarkFastAtof()
{
    const Benchmark_CmpArg arg = {
        "str->float",
        &TestCore_run_fast_atof,
        &TestCore_run_strtof
    };
    TestCore_fillFloatArray();
    const std::vector<Benchmark_CmpResult> results =
            Benchmark_cmpBatch(2, makeSpan(&arg, 1));
    std::cout << '\n';
    Benchmark_printResults(
                std::cout,
                Benchmark_PrintFormat::Markdown,
                { "fast_atof", "strtof" },
                results);

#ifndef GMIO_DEBUG_BUILD // Check only for release builds
    UTEST_ASSERT((1.05*results[0].func1_exec_time.ms) < results[0].func2_exec_time.ms);
#endif

    return nullptr;
}

} // namespace gmio
