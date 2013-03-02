#ifndef FOUG_C_LIBSTL_STLA_READ_H
#define FOUG_C_LIBSTL_STLA_READ_H

#include "stl_global.h"
#include "stl_triangle.h"
#include "../stream.h"
#include "../task_control.h"

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
typedef struct foug_stla_read_args
{
  foug_stla_geom_input_t geom_input;
  foug_stream_t stream;
  foug_task_control_t task_control;
  char* buffer;
  uint32_t buffer_size;
  size_t data_size_hint;
} foug_stla_read_args_t;

FOUG_DATAEX_LIBSTL_EXPORT
int foug_stla_read(foug_stla_read_args_t* args);

/* Error codes returned by foug_stlb_read() */
#define FOUG_STLA_READ_NO_ERROR                  0
#define FOUG_STLA_READ_NULL_BUFFER               3
#define FOUG_STLA_READ_INVALID_BUFFER_SIZE_ERROR 4
#define FOUG_STLA_READ_PARSE_ERROR               5
#define FOUG_STLA_READ_STREAM_ERROR              7
#define FOUG_STLA_READ_TASK_STOPPED_ERROR        8

#endif /* FOUG_C_LIBSTL_STLA_READ_H */
