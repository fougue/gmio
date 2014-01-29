#include "bench_tools.h"

#include <stdio.h>

float elapsed_secs(clock_t start_tick)
{
  return (float)((clock() - start_tick) / (float)CLOCKS_PER_SEC);
}

void benchmark(void (*func)(const char*), const char* title, int argc, char** argv)
{
  const clock_t start_tick = clock();
  int iarg;

  if (func == NULL)
    return;

  printf("Bench %s ...\n", title);
  for (iarg = 0; iarg < argc; ++iarg) {
    printf("  Read file %s ...\n", argv[iarg]);
    (*func)(argv[iarg]);
  }
  printf("\n  %s exec time: %.2fs\n", title, elapsed_secs(start_tick));
}
