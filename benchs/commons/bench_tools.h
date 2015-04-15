#ifndef BENCH_TOOLS_H
#define BENCH_TOOLS_H

#include <time.h>

#include <gmio_core/global.h>

GMIO_C_LINKAGE_BEGIN

float elapsed_secs(clock_t start_tick);

typedef void (*bench_file_func_t)(const char*);

void benchmark_list(
        bench_file_func_t func, const char* title, int argc, char** argv);

void benchmark(
        bench_file_func_t func, const char* title, const char* filepath);

GMIO_C_LINKAGE_END

#endif /* BENCH_TOOLS_H */
