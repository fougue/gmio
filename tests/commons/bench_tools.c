#include "bench_tools.h"

#include <stdio.h>

float elapsed_secs(clock_t start_tick)
{
  return (float)((clock() - start_tick) / (float)CLOCKS_PER_SEC);
}

void benchmark(void (*func)(const char*), const char* title, int argc, char** argv)
{
  if (func == NULL)
    return;

  const clock_t start_tick = clock();

  fprintf(stdout, "Bench %s ...\n", title);
  int iarg;
  for (iarg = 0; iarg < argc; ++iarg) {
    fprintf(stdout, "  Read file %s ...\n", argv[iarg]);
    (*func)(argv[iarg]);
  }
  fprintf(stdout, "\n  %s exec time: %.2fs\n", title, elapsed_secs(start_tick));
}
