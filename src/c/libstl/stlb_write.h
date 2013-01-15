#ifndef FOUG_C_LIBSTL_STLB_WRITE_H
#define FOUG_C_LIBSTL_STLB_WRITE_H

#include "stl_global.h"
#include "triangle.h"
#include "../stream.h"
#include "../task_control.h"

/* foug_stlb_geom_output : opaque structure */
typedef struct _internal_foug_stlb_geom_output foug_stlb_geom_output_t;
typedef void (*foug_stlb_geom_output_get_header_func_t)(const foug_stlb_geom_output_t*, uint8_t*);
typedef uint32_t (*foug_stlb_geom_output_get_triangle_count_func_t)(const foug_stlb_geom_output_t*);
typedef void (*foug_stlb_geom_output_get_triangle_func_t)(const foug_stlb_geom_output_t*,
                                                          uint32_t,
                                                          foug_stl_triangle_t*,
                                                          uint16_t*);

typedef struct
{
  foug_stlb_geom_output_get_header_func_t get_header_func;
  foug_stlb_geom_output_get_triangle_count_func_t get_triangle_count_func;
  foug_stlb_geom_output_get_triangle_func_t get_triangle_func;
} foug_stlb_geom_output_manip_t;

FOUG_DATAEX_LIBSTL_EXPORT
foug_stlb_geom_output_t* foug_stlb_geom_output_create(foug_malloc_func_t func,
                                                      void* data,
                                                      foug_stlb_geom_output_manip_t manip);

FOUG_DATAEX_LIBSTL_EXPORT
void* foug_stlb_geom_output_get_cookie(const foug_stlb_geom_output_t* geom);

typedef struct
{
  foug_stlb_geom_output_t* geom_output;
  foug_stream_t* stream;
  foug_task_control_t* task_control;
  uint32_t buffer_size;
} foug_stlb_write_args_t;

FOUG_DATAEX_LIBSTL_EXPORT
int foug_stlb_write(foug_stlb_write_args_t args);

#define FOUG_STLB_WRITE_NO_ERROR 0
#define FOUG_STLB_WRITE_NULL_GEOM_OUTPUT_ERROR 1
#define FOUG_STLB_WRITE_NULL_STREAM_ERROR 2
#define FOUG_STLB_WRITE_NULL_GET_TRIANGLE_COUNT_FUNC 3
#define FOUG_STLB_WRITE_NULL_GET_TRIANGLE_FUNC 4
#define FOUG_STLB_WRITE_STREAM_ERROR 5
#define FOUG_STLB_WRITE_TASK_STOPPED_ERROR 6
#define FOUG_STLB_WRITE_INVALID_BUFFER_SIZE_ERROR 7

#endif /* FOUG_C_LIBSTL_STLB_WRITE_H */
