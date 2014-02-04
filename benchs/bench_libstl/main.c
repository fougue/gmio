#include <datax/libstl/stla_read.h>
#include <datax/libstl/stlb_read.h>
#include <datax/error.h>

#include "../commons/bench_tools.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct my_igeom
{
  uint32_t facet_count;
} my_igeom_t;

static void dummy_process_triangle(void* cookie,
                                   uint32_t triangle_id,
                                   const foug_stl_triangle_t* triangle)
{
  my_igeom_t* my_igeom = (my_igeom_t*)(cookie);
  if (my_igeom != NULL)
    ++(my_igeom->facet_count);
}

static void libstl_foug_stlb_read(const char* filepath)
{
  my_igeom_t cookie;
  foug_transfer_t trsf;
  foug_stlb_geom_input_t geom;
  int result;

  FILE* file = fopen(filepath, "rb");
  if (file == NULL) {
    fprintf(stderr, "Failed to open binary STL file %s\n", filepath);
    return;
  }

  cookie.facet_count = 0;
  memset(&geom, 0, sizeof(foug_stlb_geom_input_t));
  geom.cookie = &cookie;
  geom.process_triangle_func = (foug_stlb_process_triangle_func_t)dummy_process_triangle;

  memset(&trsf, 0, sizeof(foug_transfer_t));
  foug_stream_set_stdio(&trsf.stream, file);
  trsf.buffer = (uint8_t*)malloc(512 * 1024);
  trsf.buffer_size = 512 * 1024;

  result = foug_stlb_read(&geom, &trsf, FOUG_LITTLE_ENDIAN);
  if (foug_datax_error(result))
    fprintf(stderr, "foug_stlb_read() error %i", result);

  fprintf(stdout, "Facet count: %i\n", cookie.facet_count);

  free(trsf.buffer);
  fclose(file);
}

static void libstl_foug_stla_read(const char* filepath)
{
  foug_transfer_t trsf;
  int result;
  FILE* file;

  file = fopen(filepath, "r");
  if (file == NULL) {
    fprintf(stderr, "Failed to open ascii STL file %s\n", filepath);
    return;
  }

  memset(&trsf, 0, sizeof(foug_transfer_t));
  foug_stream_set_stdio(&trsf.stream, file);
  trsf.buffer = (char*)malloc(512 * 1024);
  trsf.buffer_size = 512 * 1024;

  result = foug_stla_read(NULL, &trsf, 0);
  if (foug_datax_error(result))
    fprintf(stderr, "foug_stla_read() error %i", result);

  /* fprintf(stdout, "Facet count: %i\n", igeom.facet_count); */

  fclose(file);
}

int main(int argc, char** argv)
{
  if (argc < 3)
    return -1;

  if (strcmp(argv[1], "--stla") == 0)
    benchmark(&libstl_foug_stla_read, "foug_stla_read()", argc - 2, argv + 2);
  else if (strcmp(argv[1], "--stlb") == 0)
    benchmark(&libstl_foug_stlb_read, "foug_stlb_read()", argc - 2, argv + 2);

  return 0;
}