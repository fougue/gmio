#include "stlb_read.h"

#include "../endian.h"

struct _internal_foug_stlb_geom_input
{
  void* cookie;
  foug_stlb_geom_input_manip_t manip;
};

foug_stlb_geom_input_t* foug_stlb_geom_input_create(foug_malloc_func_t func,
                                                    void* data,
                                                    foug_stlb_geom_input_manip_t manip)
{
  foug_stlb_geom_input_t* geom;

  if (func == NULL)
    return NULL;
  geom = (*func)(sizeof(struct _internal_foug_stlb_geom_input));
  if (geom != NULL) {
    geom->cookie = data;
    geom->manip = manip;
  }
  return geom;
}

void* foug_stlb_geom_input_get_cookie(const foug_stlb_geom_input_t* geom)
{
  return geom != NULL ? geom->cookie : NULL;
}

static foug_bool_t foug_stlb_no_error(int code)
{
  return code == FOUG_STLB_READ_NO_ERROR;
}

static void foug_stlb_read_facets(foug_stlb_geom_input_t* geom_input,
                                  uint8_t* buffer,
                                  uint32_t facet_count)
{
  foug_stl_triangle_t triangle;
  uint16_t attr_byte_count;
  uint32_t buffer_offset;
  uint32_t i_facet;

  if (geom_input->manip.process_next_triangle_func == NULL)
    return;

  buffer_offset = 0;
  for (i_facet = 0; i_facet < facet_count; ++i_facet) {
    /* Read normal */
    triangle.normal.x = foug_decode_real32_le(buffer + buffer_offset);
    triangle.normal.y = foug_decode_real32_le(buffer + 1*sizeof(foug_real32_t) + buffer_offset);
    triangle.normal.z = foug_decode_real32_le(buffer + 2*sizeof(foug_real32_t) + buffer_offset);

    /* Read vertex1 */
    triangle.v1.x = foug_decode_real32_le(buffer + 3*sizeof(foug_real32_t) + buffer_offset);
    triangle.v1.y = foug_decode_real32_le(buffer + 4*sizeof(foug_real32_t) + buffer_offset);
    triangle.v1.z = foug_decode_real32_le(buffer + 5*sizeof(foug_real32_t) + buffer_offset);

    /* Read vertex2 */
    triangle.v2.x = foug_decode_real32_le(buffer + 6*sizeof(foug_real32_t) + buffer_offset);
    triangle.v2.y = foug_decode_real32_le(buffer + 7*sizeof(foug_real32_t) + buffer_offset);
    triangle.v2.z = foug_decode_real32_le(buffer + 8*sizeof(foug_real32_t) + buffer_offset);

    /* Read vertex3 */
    triangle.v3.x = foug_decode_real32_le(buffer + 9*sizeof(foug_real32_t) + buffer_offset);
    triangle.v3.y = foug_decode_real32_le(buffer + 10*sizeof(foug_real32_t) + buffer_offset);
    triangle.v3.z = foug_decode_real32_le(buffer + 11*sizeof(foug_real32_t) + buffer_offset);

    /* Attribute byte count */
    attr_byte_count = foug_decode_uint16_le(buffer + 12*sizeof(foug_real32_t) + buffer_offset);

    /* Add triangle */
    (*(geom_input->manip.process_next_triangle_func))(geom_input, &triangle, attr_byte_count);
    buffer_offset += FOUG_STLB_TRIANGLE_SIZE;
  }
}

int foug_stlb_read(foug_stlb_read_args_t args)
{
  uint8_t buffer[8192];
  uint8_t header_data[FOUG_STLB_HEADER_SIZE];
  uint32_t total_facet_count;
  size_t buffer_facet_count;
  size_t accum_facet_count_read;
  int error;
  size_t facet_count_read;

  if (args.geom_input == NULL)
    return FOUG_STLB_READ_NULL_GEOM_INPUT_ERROR;
  if (args.stream == NULL)
    return FOUG_STLB_READ_NULL_STREAM_ERROR;
/*  if (args.buffer_size == 0)
      return FOUG_STLB_READ_INVALID_BUFFER_SIZE_ERROR;*/

  /* Read header */
  if (foug_stream_read(args.stream, header_data, 1, FOUG_STLB_HEADER_SIZE) != FOUG_STLB_HEADER_SIZE)
    return FOUG_STLB_READ_HEADER_WRONG_SIZE_ERROR;

  if (args.geom_input->manip.process_header_func != NULL)
    (*(args.geom_input->manip.process_header_func))(args.geom_input, header_data);

  /* Read facet count */
  if (foug_stream_read(args.stream, buffer, sizeof(uint32_t), 1) != 1)
    return FOUG_STLB_READ_FACET_COUNT_ERROR;

  total_facet_count = foug_decode_uint32_le(buffer);
  if (args.geom_input->manip.begin_triangles_func != NULL)
    (*(args.geom_input->manip.begin_triangles_func))(args.geom_input, total_facet_count);

  foug_task_control_reset(args.task_control);
  foug_task_control_set_range(args.task_control, 0., (foug_real32_t)total_facet_count);

  /* Read triangles */
  buffer_facet_count = 163;
  accum_facet_count_read = 0;
  error = FOUG_STLB_READ_NO_ERROR;
  while (foug_stlb_no_error(error) && accum_facet_count_read < total_facet_count) {
    facet_count_read = foug_stream_read(args.stream,
                                        buffer, FOUG_STLB_TRIANGLE_SIZE, buffer_facet_count);
    if (foug_stream_error(args.stream) != 0)
      error = FOUG_STLB_READ_STREAM_ERROR;
    else if (facet_count_read > 0)
      error = FOUG_STLB_READ_NO_ERROR;
    else
      break; /* Exit if no facet to read */

    if (foug_stlb_no_error(error)) {
      foug_stlb_read_facets(args.geom_input, buffer, facet_count_read);
      accum_facet_count_read += facet_count_read;
      if (foug_task_control_is_stop_requested(args.task_control)) {
        error = FOUG_STLB_READ_TASK_STOPPED_ERROR;
        foug_task_control_handle_stop(args.task_control);
      }
      else {
        foug_task_control_set_progress(args.task_control, accum_facet_count_read);
      }
    }
  } /* end while */

  if (foug_stlb_no_error(error) && args.geom_input->manip.end_triangles_func != NULL)
    (*(args.geom_input->manip.end_triangles_func))(args.geom_input);

  if (foug_stlb_no_error(error) && accum_facet_count_read != total_facet_count)
    error = FOUG_STLB_READ_FACET_COUNT_ERROR;
  return error;
}
