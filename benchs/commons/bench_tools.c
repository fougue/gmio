#include "bench_tools.h"

#include <stdio.h>

float elapsed_secs(clock_t start_tick)
{
    return (float)((clock() - start_tick) / (float)CLOCKS_PER_SEC);
}

void benchmark_list(bench_file_func_t func, const char* title, int argc, char **argv)
{
    const clock_t start_tick = clock();
    int iarg;

    if (func == NULL)
        return;

    printf("Bench %s ...\n", title);
    for (iarg = 0; iarg < argc; ++iarg) {
        printf("  File %s ...\n", argv[iarg]);
        (*func)(argv[iarg]);
    }
    printf("  exec time: %.2fs\n\n", elapsed_secs(start_tick));
}

void benchmark(bench_file_func_t func, const char *title, const char* filepath)
{
    benchmark_list(func, title, 1, (char**)&filepath);
}
