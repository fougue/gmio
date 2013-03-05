#include "stlb_write.h"

#include "../endian.h"
#include "../error.h"
#include <string.h>

static void foug_stlb_write_facets(const foug_stlb_geom_output_t* geom_output,
                                   uint8_t* buffer,
                                   uint32_t ifacet_start,
                                   uint32_t facet_count)
{
  foug_stlb_triangle_t triangle;
  uint32_t buffer_offset;
  uint32_t i_facet;

  if (geom_output->get_triangle_func == NULL)
    return;

  buffer_offset = 0;
  for (i_facet = ifacet_start; i_facet < (ifacet_start + facet_count); ++i_facet) {
    geom_output->get_triangle_func(geom_output, i_facet, &triangle);

    memcpy(buffer + buffer_offset, &triangle, FOUG_STLB_TRIANGLE_SIZE);
    buffer_offset += FOUG_STLB_TRIANGLE_SIZE;
  } /* end for */
}

int foug_stlb_write(foug_stlb_write_args_t *args)
{
  uint32_t facet_count;
  uint32_t i_facet;
  uint32_t buffer_facet_count;
  uint32_t ifacet_start;
  int error;

  if (args->buffer == NULL)
    return FOUG_DATAX_NULL_BUFFER_ERROR;
  if (args->buffer_size < FOUG_STLB_MIN_CONTENTS_SIZE)
    return FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;
  if (args->geom_output.get_triangle_count_func == NULL)
    return FOUG_STLB_WRITE_NULL_GET_TRIANGLE_COUNT_FUNC;
  if (args->geom_output.get_triangle_func == NULL)
    return FOUG_STLB_WRITE_NULL_GET_TRIANGLE_FUNC;

  /* Write header */
  {
    uint8_t header_data[FOUG_STLB_HEADER_SIZE];
    if (args->geom_output.get_header_func != NULL)
      args->geom_output.get_header_func(&args->geom_output, header_data);
    else
      memset(header_data, 0, FOUG_STLB_HEADER_SIZE);

    if (foug_stream_write(&args->stream, header_data, FOUG_STLB_HEADER_SIZE, 1) != 1)
      return FOUG_DATAX_STREAM_ERROR;
  }

  /* Write facet count */
  facet_count = args->geom_output.get_triangle_count_func(&args->geom_output);
  foug_encode_uint32_le(facet_count, args->buffer);
  if (foug_stream_write(&args->stream, args->buffer, sizeof(uint32_t), 1) != 1)
    return FOUG_DATAX_STREAM_ERROR;

  /* Write triangles */
  error = FOUG_DATAX_NO_ERROR;

  buffer_facet_count = args->buffer_size / FOUG_STLB_TRIANGLE_SIZE;
  ifacet_start = 0;
  for (i_facet = 0;
       i_facet < facet_count && foug_datax_no_error(error);
       i_facet += buffer_facet_count)
  {
    /* Write to buffer */
    if (buffer_facet_count > (facet_count - ifacet_start))
      buffer_facet_count = facet_count - ifacet_start;
    foug_stlb_write_facets(&args->geom_output, args->buffer, ifacet_start, buffer_facet_count);
    ifacet_start += buffer_facet_count;

    /* Write buffer to stream */
    if (foug_stream_write(&args->stream, args->buffer, FOUG_STLB_TRIANGLE_SIZE, buffer_facet_count)
        != buffer_facet_count)
    {
      error = FOUG_DATAX_STREAM_ERROR;
    }

    /* Task control */
    if (foug_datax_no_error(error)
        && !foug_task_control_handle_progress(&args->task_control,
                                              foug_percentage(0, facet_count, i_facet + 1)))
    {
      error = FOUG_DATAX_TASK_STOPPED_ERROR;
    }
  } /* end for */

  return error;
}
