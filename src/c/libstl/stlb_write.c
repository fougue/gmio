#include "stlb_write.h"

#include "../endian.h"
#include <string.h>

struct _internal_foug_stlb_geom_output
{
  void* cookie;
  foug_stlb_geom_output_manip_t manip;
};

foug_stlb_geom_output_t* foug_stlb_geom_output_create(foug_malloc_func_t func,
                                                      void* data,
                                                      foug_stlb_geom_output_manip_t manip)
{
  foug_stlb_geom_output_t* geom;

  if (func == NULL)
    return NULL;
  geom = (*func)(sizeof(struct _internal_foug_stlb_geom_output));
  if (geom != NULL) {
    geom->cookie = data;
    geom->manip = manip;
  }
  return geom;
}

void* foug_stlb_geom_output_get_cookie(const foug_stlb_geom_output_t* geom)
{
  return geom != NULL ? geom->cookie : NULL;
}

void foug_stlb_geom_output_set_cookie(foug_stlb_geom_output_t* geom, void* data)
{
  if (geom != NULL)
    geom->cookie = data;
}

static foug_bool_t foug_stlb_no_error(int code)
{
  return code == FOUG_STLB_WRITE_NO_ERROR;
}

static void foug_stlb_write_facets(const foug_stlb_geom_output_t* geom_output,
                                   uint8_t* buffer,
                                   uint32_t ifacet_start,
                                   uint32_t facet_count)
{
  foug_stl_triangle_t triangle;
  uint16_t attr_byte_count;
  uint32_t buffer_offset;
  uint32_t i_facet;

  if (geom_output->manip.get_triangle_func == NULL)
    return;

  attr_byte_count = 0;
  buffer_offset = 0;
  for (i_facet = ifacet_start; i_facet < (ifacet_start + facet_count); ++i_facet) {
    (*(geom_output->manip.get_triangle_func))(geom_output, i_facet, &triangle, &attr_byte_count);

    /* Normal */
    foug_encode_real32_le(triangle.normal.x, buffer + buffer_offset);
    foug_encode_real32_le(triangle.normal.y, buffer + 1*sizeof(foug_real32_t) + buffer_offset);
    foug_encode_real32_le(triangle.normal.z, buffer + 2*sizeof(foug_real32_t) + buffer_offset);

    /* Vertex1 */
    foug_encode_real32_le(triangle.v1.x, buffer + 3*sizeof(foug_real32_t) + buffer_offset);
    foug_encode_real32_le(triangle.v1.y, buffer + 4*sizeof(foug_real32_t) + buffer_offset);
    foug_encode_real32_le(triangle.v1.z, buffer + 5*sizeof(foug_real32_t) + buffer_offset);

    /* Vertex2 */
    foug_encode_real32_le(triangle.v2.x, buffer + 6*sizeof(foug_real32_t) + buffer_offset);
    foug_encode_real32_le(triangle.v2.y, buffer + 7*sizeof(foug_real32_t) + buffer_offset);
    foug_encode_real32_le(triangle.v2.z, buffer + 8*sizeof(foug_real32_t) + buffer_offset);

    /* Vertex3 */
    foug_encode_real32_le(triangle.v3.x, buffer + 9*sizeof(foug_real32_t) + buffer_offset);
    foug_encode_real32_le(triangle.v3.y, buffer + 10*sizeof(foug_real32_t) + buffer_offset);
    foug_encode_real32_le(triangle.v3.z, buffer + 11*sizeof(foug_real32_t) + buffer_offset);

    /* Attribute byte count */
    foug_encode_uint16_le(attr_byte_count, buffer + 12*sizeof(foug_real32_t) + buffer_offset);

    buffer_offset += FOUG_STLB_TRIANGLE_SIZE;
  } /* end for */
}

int foug_stlb_write(foug_stlb_write_args_t args)
{
  uint8_t header_data[FOUG_STLB_HEADER_SIZE];
  uint32_t facet_count;
  uint32_t i_facet;
  uint32_t buffer_facet_count;
  uint32_t ifacet_start;
  int error;

  if (args.geom_output == NULL)
    return FOUG_STLB_WRITE_NULL_GEOM_OUTPUT_ERROR;
  if (args.stream == NULL)
    return FOUG_STLB_WRITE_NULL_STREAM_ERROR;
  if (args.buffer == NULL)
    return FOUG_STLB_WRITE_NULL_BUFFER_ERROR;
  if (args.buffer_size < FOUG_STLB_MIN_CONTENTS_SIZE)
    return FOUG_STLB_WRITE_INVALID_BUFFER_SIZE_ERROR;
  if (args.geom_output->manip.get_triangle_count_func == NULL)
    return FOUG_STLB_WRITE_NULL_GET_TRIANGLE_COUNT_FUNC;
  if (args.geom_output->manip.get_triangle_func == NULL)
    return FOUG_STLB_WRITE_NULL_GET_TRIANGLE_FUNC;

  /* Write header */
  if (args.geom_output->manip.get_header_func != NULL)
    (*(args.geom_output->manip.get_header_func))(args.geom_output, header_data);
  else
    memset(header_data, 0, FOUG_STLB_HEADER_SIZE);

  if (foug_stream_write(args.stream, header_data, FOUG_STLB_HEADER_SIZE, 1) != 1)
    return FOUG_STLB_WRITE_STREAM_ERROR;

  /* Write facet count */
  facet_count = (*(args.geom_output->manip.get_triangle_count_func))(args.geom_output);
  foug_encode_uint32_le(facet_count, args.buffer);
  if (foug_stream_write(args.stream, args.buffer, sizeof(uint32_t), 1) != 1)
    return FOUG_STLB_WRITE_STREAM_ERROR;

  foug_task_control_reset(args.task_control);
  foug_task_control_set_range(args.task_control, 0., (foug_real32_t)facet_count);

  /* Write triangles */
  error = FOUG_STLB_WRITE_NO_ERROR;

  buffer_facet_count = args.buffer_size / FOUG_STLB_TRIANGLE_SIZE;
  ifacet_start = 0;
  for (i_facet = 0;
       i_facet < facet_count && foug_stlb_no_error(error);
       i_facet += buffer_facet_count)
  {
    /* Write to buffer */
    if (buffer_facet_count > (facet_count - ifacet_start))
      buffer_facet_count = facet_count - ifacet_start;
    foug_stlb_write_facets(args.geom_output, args.buffer, ifacet_start, buffer_facet_count);
    ifacet_start += buffer_facet_count;

    /* Write buffer to stream */
    if (foug_stream_write(args.stream, args.buffer, FOUG_STLB_TRIANGLE_SIZE, buffer_facet_count)
        != buffer_facet_count)
    {
      error = FOUG_STLB_WRITE_STREAM_ERROR;
    }

    /* Task control */
    if (foug_stlb_no_error(error)) {
      if (foug_task_control_is_stop_requested(args.task_control)) {
        foug_task_control_handle_stop(args.task_control);
        error = FOUG_STLB_WRITE_TASK_STOPPED_ERROR;
      }
      else {
        foug_task_control_set_progress(args.task_control,
                                       (foug_real32_t)(i_facet + 1));
      }
    }
  } /* end for */

  return error;
}
