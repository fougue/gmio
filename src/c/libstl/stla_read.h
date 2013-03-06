#ifndef FOUG_DATAX_C_LIBSTL_STLA_READ_H
#define FOUG_DATAX_C_LIBSTL_STLA_READ_H

#include "stl_global.h"
#include "stl_triangle.h"
#include "../error.h"
#include "../stream.h"
#include "../task_control.h"

/* foug_stla_read() */
typedef struct foug_stla_read_args foug_stla_read_args_t;

FOUG_DATAX_LIBSTL_EXPORT int foug_stla_read(foug_stla_read_args_t* args);

/* Specific error codes returned by foug_stla_read() */
#define FOUG_STLA_READ_PARSE_ERROR  1

/* foug_stla_geom_input */
typedef struct foug_stla_geom_input foug_stla_geom_input_t;
struct foug_stla_geom_input
{
  void* cookie;
  void (*begin_solid_func)(foug_stla_geom_input_t*, const char* name);
  void (*process_next_triangle_func)(foug_stla_geom_input_t*, const foug_stl_triangle_t*);
  void (*end_solid_func)(foug_stla_geom_input_t*, const char* name);
  /* void (*parse_error_func)(foug_stla_geom_input_t*, size_t, size_t); */
};

/* foug_stla_read_args */
struct foug_stla_read_args
{
  foug_stla_geom_input_t geom_input;
  foug_stream_t stream;
  foug_task_control_t task_control;
  char* buffer;
  uint32_t buffer_size;
  size_t data_size_hint;
};

#endif /* FOUG_DATAX_C_LIBSTL_STLA_READ_H */
