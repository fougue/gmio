#ifndef BENCH_TOOLS_H
#define BENCH_TOOLS_H

#include <time.h>

#include <gmio_core/global.h>

GMIO_C_LINKAGE_BEGIN

float elapsed_secs(clock_t start_tick);

void benchmark(
        void (*func)(const char*), const char* title, int argc, char** argv);

GMIO_C_LINKAGE_END

#endif /* BENCH_TOOLS_H */
