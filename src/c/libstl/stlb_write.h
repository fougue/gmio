#ifndef FOUG_C_LIBSTL_STLB_WRITE_H
#define FOUG_C_LIBSTL_STLB_WRITE_H

#include "stl_global.h"
#include "stlb_triangle.h"
#include "../stream.h"
#include "../task_control.h"

/* foug_stlb_geom_output */
typedef struct foug_stlb_geom_output foug_stlb_geom_output_t;
struct foug_stlb_geom_output
{
  void* cookie;
  void (*get_header_func)(const foug_stlb_geom_output_t*, uint8_t*);
  uint32_t (*get_triangle_count_func)(const foug_stlb_geom_output_t*);
  void (*get_triangle_func)(const foug_stlb_geom_output_t*, uint32_t, foug_stlb_triangle_t*);
};

/* foug_stlb_write_args */
typedef struct foug_stlb_write_args
{
  foug_stlb_geom_output_t geom_output;
  foug_stream_t stream;
  foug_task_control_t task_control;
  uint8_t* buffer;
  uint32_t buffer_size;
} foug_stlb_write_args_t;

FOUG_DATAX_LIBSTL_EXPORT
int foug_stlb_write(foug_stlb_write_args_t* args);

/* Error codes returned by foug_stlb_write() */
#define FOUG_STLB_WRITE_NO_ERROR                     0
#define FOUG_STLB_WRITE_NULL_BUFFER_ERROR            3
#define FOUG_STLB_WRITE_NULL_GET_TRIANGLE_COUNT_FUNC 4
#define FOUG_STLB_WRITE_NULL_GET_TRIANGLE_FUNC       5
#define FOUG_STLB_WRITE_STREAM_ERROR                 6
#define FOUG_STLB_WRITE_TASK_STOPPED_ERROR           7
#define FOUG_STLB_WRITE_INVALID_BUFFER_SIZE_ERROR    8

#endif /* FOUG_C_LIBSTL_STLB_WRITE_H */
