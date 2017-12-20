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

#define NOMINMAX

#include "benchmark_tools.h"

#include <algorithm>
#include <limits>
#include <ostream>
#include <iomanip>
#include <sstream>

#ifdef GMIO_OS_WIN
#  include <windows.h>
#  define BENCHMARK_TIMER_WINDOWS
#else
#  include <ctime>
#  define BENCHMARK_TIMER_LIBC
#endif

namespace gmio {

// Benchmark timers

class Benchmark_Timer::Pimpl {
public:
#ifdef BENCHMARK_TIMER_WINDOWS
    LARGE_INTEGER m_start_time;
    LARGE_INTEGER m_frequency;
#elif defined(BENCHMARK_TIMER_LIBC)
    std::clock_t m_start_tick;
#endif
};

Benchmark_Timer::Benchmark_Timer()
    : d(nullptr)
{
}

Benchmark_Timer::~Benchmark_Timer()
{
    delete d;
}

void Benchmark_Timer::start() {
    if (d == nullptr)
        d = new Pimpl;
#ifdef BENCHMARK_TIMER_WINDOWS
    QueryPerformanceFrequency(&d->m_frequency);
    QueryPerformanceCounter(&d->m_start_time);
#elif defined(BENCHMARK_TIMER_LIBC)
    d->m_start_tick = std::clock();
#endif
}

uint64_t Benchmark_Timer::elapsedMs() const {
    if (d == nullptr)
        return 0;
#ifdef BENCHMARK_TIMER_WINDOWS
    LARGE_INTEGER end_time;
    LARGE_INTEGER elapsed;
    QueryPerformanceCounter(&end_time);
    elapsed.QuadPart = end_time.QuadPart - d->m_start_time.QuadPart;

    // We now have the elapsed number of ticks, along with the
    // number of ticks-per-second. We use these values
    // to convert to the number of elapsed microseconds.
    // To guard against loss-of-precision, we convert
    // to milliseconds *before* dividing by ticks-per-second.
    elapsed.QuadPart *= 1000;
    elapsed.QuadPart /= d->m_frequency.QuadPart;
    return elapsed.QuadPart;
#elif defined(BENCHMARK_TIMER_LIBC)
    const std::clock_t elapsed_ticks =
            std::clock() - d->m_start_tick;
    const double elapsed_ms =
            (elapsed_ticks * 1000) / (static_cast<double>(CLOCKS_PER_SEC));
    return static_cast<uint64_t>(elapsed_ms);
#endif
}

// Utilities

//! Prints 'str' on stream 'n' times
static std::ostream& print_string_n(std::ostream& ostr, const char* str, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        ostr << str;
    return ostr;
}

//! Formatted print of some execution time
static std::ostream& print_exec_time(
        std::ostream& outs,
        size_t width_column,
        const Benchmark_ExecTime& time)
{
    const std::string str_time =
            time.is_valid ? (std::to_string(time.ms) + "ms") : "N/A";
    outs << std::setw(width_column) << str_time;
    return outs;
}

//! Formatted print of a ratio
static std::ostream& print_exec_ratio(
        std::ostream& outs,
        size_t width_column,
        float ratio)
{
    if (!(ratio < 0)) // Valid ratio ?
        outs << std::setw(width_column) << std::setprecision(4) << ratio;
    else
        outs << std::setw(width_column) << "N/A";
    return outs;
}

//! Returns the strlen of the longest tag string
static size_t find_maxlen_cmp_result_tag(
        Span<const Benchmark_CmpResult> results)
{
    size_t maxlen = 0;
    for (const Benchmark_CmpResult& res : results)
        maxlen = std::max(res.tag.length(), maxlen);
    return maxlen;
}

//! Returns the strlen of the longest execution time string
static size_t find_maxlen_cmp_result_exec_time(
        Span<const Benchmark_CmpResult> results,
        unsigned exec_time_col)
{
    size_t maxlen = 0;
    for (const Benchmark_CmpResult& res : results) {
        const Benchmark_ExecTime& time =
                exec_time_col == 0 ? res.func1_exec_time : res.func2_exec_time;
        std::stringstream ostr;
        print_exec_time(ostr, 0, time);
        maxlen = std::max(ostr.str().length(), maxlen);
    }
    return maxlen;
}

//! Returns the strlen of the longest func2/func1 ratio string
static size_t find_maxlen_cmp_result_ratio(
        Span<const Benchmark_CmpResult> results)
{
    size_t maxlen = 0;
    for (const Benchmark_CmpResult& res : results) {
        std::stringstream ostr;
        print_exec_ratio(ostr, 0, res.func2_func1_ratio);
        maxlen = std::max(ostr.str().length(), maxlen);
    }
    return maxlen;
}

static void update_benchmark_cmp_result_ratio(Benchmark_CmpResult* result)
{
    if (result->func1_exec_time.is_valid && result->func2_exec_time.is_valid) {
        if (result->func2_exec_time.ms > 0) {
            result->func2_func1_ratio =
                    result->func2_exec_time.ms
                    / static_cast<float>(result->func1_exec_time.ms);
        }
        else {
            result->func2_func1_ratio = std::numeric_limits<float>::max();
        }
    }
    else {
        result->func2_func1_ratio = -1.f; // Non-valid ratio
    }
}

// Implementation

Benchmark_CmpResult Benchmark_cmp(const Benchmark_CmpArg& arg)
{
    Benchmark_CmpResult result = {};
    result.tag = arg.tag;

    if (arg.func1) {
        Benchmark_Timer timer;
        timer.start();
        arg.func1();
        result.func1_exec_time.ms = timer.elapsedMs();
        result.func1_exec_time.is_valid = true;
    }
    if (arg.func2) {
        Benchmark_Timer timer;
        timer.start();
        arg.func2();
        result.func2_exec_time.ms = timer.elapsedMs();
        result.func2_exec_time.is_valid = true;
    }
    update_benchmark_cmp_result_ratio(&result);

    return result;
}

std::vector<Benchmark_CmpResult> Benchmark_cmpBatch(
        size_t run_count,
        Span<const Benchmark_CmpArg> args,
        const std::function<void ()>& func_init,
        const std::function<void ()>& func_cleanup)
{
    std::vector<Benchmark_CmpResult> results;
    results.resize(args.size());

    for (size_t run = 0; run < run_count; ++run) {
        if (func_init)
            func_init();

        for (size_t i = 0; i < args.size(); ++i) {
            const Benchmark_CmpResult ires = Benchmark_cmp(args[i]);
            Benchmark_CmpResult* fres = &results.at(i);
            if (run != 0) {
                if (fres->func1_exec_time.ms > ires.func1_exec_time.ms)
                    fres->func1_exec_time.ms = ires.func1_exec_time.ms;
                if (fres->func2_exec_time.ms > ires.func2_exec_time.ms)
                    fres->func2_exec_time.ms = ires.func2_exec_time.ms;
                update_benchmark_cmp_result_ratio(fres);
            }
            else {
                *fres = ires;
            }
        }

        if (func_cleanup)
            func_cleanup();
    }

    return results;
}

void Benchmark_printResults(
        std::ostream& ostr,
        Benchmark_PrintFormat format,
        const Benchmark_CmpResultHeader& header,
        Span<const Benchmark_CmpResult> results)
{
    if (format == Benchmark_PrintFormat::Markdown) {
        static const char header_ratio[] = "ratio";
        const size_t width_tag_col =
                find_maxlen_cmp_result_tag(results);
        const size_t width_func1_col =
                std::max(
                    find_maxlen_cmp_result_exec_time(results, 0),
                    header.component_1.length());
        const size_t width_func2_col =
                std::max(
                    find_maxlen_cmp_result_exec_time(results, 1),
                    header.component_2.length());
        const size_t width_ratio_col =
                std::max(
                    find_maxlen_cmp_result_ratio(results),
                    std::strlen(header_ratio));

        // Print table header
        ostr << std::setw(width_tag_col) << "" << " | ";
        ostr << std::setw(width_func1_col) << std::setfill(' ') << header.component_1 << " | ";
        ostr << std::setw(width_func2_col) << std::setfill(' ') << header.component_2 << " | ";
        ostr << std::setw(width_ratio_col) << std::setfill(' ') << header_ratio << "\n";

        // Print separation between header and results
        print_string_n(ostr, "-", width_tag_col + 1) << "|";
        print_string_n(ostr, "-", width_func1_col + 2) << "|";
        print_string_n(ostr, "-", width_func2_col + 2) << "|";
        print_string_n(ostr, "-", width_ratio_col + 2) << "\n";

        // Print benchmark result lines
        for (const Benchmark_CmpResult& res : results) {
            ostr.fill(' ');
            ostr << std::left << std::setw(width_tag_col) << res.tag << " | ";
            ostr << std::right;
            print_exec_time(ostr, width_func1_col, res.func1_exec_time) << " | ";
            print_exec_time(ostr, width_func2_col, res.func2_exec_time) << " | ";
            print_exec_ratio(ostr, width_ratio_col, res.func2_func1_ratio) << "\n";
        }
    }
}

void Benchmark_printResults_Markdown(
        std::ostream &ostr,
        const Benchmark_CmpResultHeader &header,
        Span<const Benchmark_CmpResult> results)
{
    Benchmark_printResults(ostr, Benchmark_PrintFormat::Markdown, header, results);
}

} // namespace gmio
