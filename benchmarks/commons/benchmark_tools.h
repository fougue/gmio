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
#include "../../src/gmio_core/span.h"

#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <vector>

namespace gmio {

class Benchmark_Timer {
public:
    Benchmark_Timer();
    ~Benchmark_Timer();

    void start();
    uint64_t elapsedMs() const;

private:
    class Pimpl;
    Pimpl* d;
};

// benchmark_cmp

//! Describes a comparison benchmark between two functions
struct Benchmark_CmpArg {
    std::string tag; //! Brief description of the comparison(eg. "Write to file")
    std::function<void ()> func1; //! Pointer to the 1st function
    std::function<void ()> func2; //! Pointer to the 2nd function
};

struct Benchmark_ExecTime {
    uint64_t ms;   //! Execution time(in ms)
    bool is_valid; //! Is exec time valid ?
};

//! Holds the result of the exec time comparison between two functions
struct Benchmark_CmpResult {
    std::string tag; //! Identifies the comparison
    Benchmark_ExecTime func1_exec_time;
    Benchmark_ExecTime func2_exec_time;
    float func2_func1_ratio;
};

//! Runs func1 then func2 and measures the respective execution time
Benchmark_CmpResult Benchmark_cmp(const Benchmark_CmpArg& arg);

//! Runs a batch(array) of comparison benchmarks
std::vector<Benchmark_CmpResult> Benchmark_cmpBatch(
        size_t run_count,
        Span<const Benchmark_CmpArg> args,
        const std::function<void ()>& func_init = nullptr,
        const std::function<void ()>& func_cleanup = nullptr);

// benchmark_print_results

enum class Benchmark_PrintFormat {
    Markdown = 0
};

//! Horizontal header labels for benchmark results(by column)
struct Benchmark_CmpResultHeader {
    std::string component_1;
    std::string component_2;
};

//! Prints formatted benchmark results
void Benchmark_printResults(
        std::ostream& ostr,
        Benchmark_PrintFormat format,
        const Benchmark_CmpResultHeader& header,
        Span<const Benchmark_CmpResult> results);

void Benchmark_printResults_Markdown(
        std::ostream& ostr,
        const Benchmark_CmpResultHeader& header,
        Span<const Benchmark_CmpResult> results);

} // namespace gmio
