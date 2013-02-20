#ifndef BENCH_TOOLS_H
#define BENCH_TOOLS_H

#include <time.h>

float elapsed_secs(clock_t start_tick);

void benchmark(void (*func)(const char*), const char* title, int argc, char** argv);

#endif /* BENCH_TOOLS_H */
