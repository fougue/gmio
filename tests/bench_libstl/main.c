#include <dataex/c/libstl/stlb_read.h>

#include "../commons/bench_tools.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct my_igeom
{
  uint32_t facet_count;
} my_igeom_t;

static void dummy_process_next_triangle(foug_stlb_geom_input_t* igeom,
                                        const foug_stlb_triangle_t* triangle)
{
  my_igeom_t* my_igeom = (my_igeom_t*)foug_stlb_geom_input_get_cookie(igeom);
  if (my_igeom != NULL)
    ++(my_igeom->facet_count);
}

static void libstl_foug_stlb_read(const char* filepath)
{
  FILE* file = fopen(filepath, "r");
  if (file == NULL) {
    fprintf(stderr, "Failed to open STL file %s\n", filepath);
    return;
  }

  foug_stlb_geom_input_manip_t igeom_manip;
  memset(&igeom_manip, 0, sizeof(foug_stlb_geom_input_manip_t));
/*  igeom_manip.process_header_func = igeom_process_header; */
/*  igeom_manip.begin_triangles_func = igeom_begin_triangles; */
  igeom_manip.process_next_triangle_func = dummy_process_next_triangle;
/*  igeom_manip.end_triangles_func = NULL; */

  my_igeom_t igeom;
  igeom.facet_count = 0;

  foug_stlb_read_args_t args;
  args.geom_input = foug_stlb_geom_input_create(malloc, &igeom, igeom_manip);
  args.stream = foug_stream_create(&malloc, file, foug_stream_manip_stdio());;
  args.task_control = NULL;

  args.buffer = (uint8_t*)malloc(512 * 1024);
  args.buffer_size = 512 * 1024;

  int result = foug_stlb_read(args);
  if (result != FOUG_STLB_READ_NO_ERROR)
    fprintf(stderr, "foug_stlb_read() error %i", result);

  fprintf(stdout, "Facet count: %i\n", igeom.facet_count);

  free(args.geom_input);
  free(args.stream);
  free(args.buffer);
  fclose(file);
}

int main(int argc, char** argv)
{
  if (argc > 1)
    benchmark(&libstl_foug_stlb_read, "foug_stlb_read()", argc - 1, argv + 1);
  return 0;
}
