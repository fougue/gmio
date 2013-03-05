#ifndef FOUG_C_LIBSTL_STLB_WRITE_H
#define FOUG_C_LIBSTL_STLB_WRITE_H

#include "stl_global.h"
#include "stlb_triangle.h"
#include "../error.h"
#include "../stream.h"
#include "../task_control.h"

/* foug_stlb_write() */
typedef struct foug_stlb_write_args foug_stlb_write_args_t;

FOUG_DATAX_LIBSTL_EXPORT int foug_stlb_write(foug_stlb_write_args_t* args);

/* Specific error codes returned by foug_stlb_write() */
#define FOUG_STLB_WRITE_NULL_GET_TRIANGLE_COUNT_FUNC 1
#define FOUG_STLB_WRITE_NULL_GET_TRIANGLE_FUNC       2

/* foug_stlb_geom_output */
typedef struct foug_stlb_geom_output foug_stlb_geom_output_t;
struct foug_stlb_geom_output
{
  void* cookie;
  void     (*get_header_func)(const foug_stlb_geom_output_t*, uint8_t*);
  uint32_t (*get_triangle_count_func)(const foug_stlb_geom_output_t*);
  void     (*get_triangle_func)(const foug_stlb_geom_output_t*, uint32_t, foug_stlb_triangle_t*);
};

/* foug_stlb_write_args */
struct foug_stlb_write_args
{
  foug_stlb_geom_output_t geom_output;
  foug_stream_t stream;
  foug_task_control_t task_control;
  uint8_t* buffer;
  uint32_t buffer_size;
};

#endif /* FOUG_C_LIBSTL_STLB_WRITE_H */
