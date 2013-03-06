#include "stlb_read.h"

#include "../error.h"
#include "../endian.h"

static void foug_stlb_read_facets(foug_stlb_geom_input_t* geom,
                                  uint8_t* buffer,
                                  uint32_t facet_count)
{
  foug_stlb_triangle_t triangle;
  uint32_t buffer_offset;
  uint32_t i_facet;

  if (geom == NULL || geom->process_next_triangle_func == NULL)
    return;

  buffer_offset = 0;
  for (i_facet = 0; i_facet < facet_count; ++i_facet) {
    /* Decode data */
    triangle = *((foug_stlb_triangle_t*)(buffer + buffer_offset));
    buffer_offset += FOUG_STLB_TRIANGLE_SIZE;

    /* Declare triangle */
    geom->process_next_triangle_func(geom, &triangle);
  }
}

int foug_stlb_read(foug_stlb_geom_input_t* geom, foug_transfer_t* trsf)
{
  uint32_t total_facet_count;
  size_t accum_facet_count_read;
  int error;

  if (trsf->buffer == NULL)
    return FOUG_DATAX_NULL_BUFFER_ERROR;
  if (trsf->buffer_size < FOUG_STLB_MIN_CONTENTS_SIZE)
    return FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;

  /* Read header */
  {
    uint8_t header_data[FOUG_STLB_HEADER_SIZE];
    if (foug_stream_read(&trsf->stream, header_data, 1, FOUG_STLB_HEADER_SIZE)
        != FOUG_STLB_HEADER_SIZE)
    {
      return FOUG_STLB_READ_HEADER_WRONG_SIZE_ERROR;
    }
    if (geom != NULL && geom->process_header_func != NULL)
      geom->process_header_func(geom, header_data);
  }

  /* Read facet count */
  if (foug_stream_read(&trsf->stream, trsf->buffer, sizeof(uint32_t), 1) != 1)
    return FOUG_STLB_READ_FACET_COUNT_ERROR;

  total_facet_count = foug_decode_uint32_le(trsf->buffer);
  if (geom != NULL && geom->begin_triangles_func != NULL)
    geom->begin_triangles_func(geom, total_facet_count);

  /* Read triangles */
  accum_facet_count_read = 0;
  error = FOUG_DATAX_NO_ERROR;
  while (foug_datax_no_error(error) && accum_facet_count_read < total_facet_count) {
    const size_t facet_count_read = foug_stream_read(&trsf->stream,
                                                     trsf->buffer,
                                                     FOUG_STLB_TRIANGLE_SIZE,
                                                     trsf->buffer_size / FOUG_STLB_TRIANGLE_SIZE);
    if (foug_stream_error(&trsf->stream) != 0)
      error = FOUG_DATAX_STREAM_ERROR;
    else if (facet_count_read > 0)
      error = FOUG_DATAX_NO_ERROR;
    else
      break; /* Exit if no facet to read */

    if (foug_datax_no_error(error)) {
      uint8_t progress_pc;

      foug_stlb_read_facets(geom, trsf->buffer, facet_count_read);
      accum_facet_count_read += facet_count_read;
      progress_pc = foug_percentage(0, total_facet_count, accum_facet_count_read);
      if (!foug_task_control_handle_progress(&trsf->task_control, progress_pc))
        error = FOUG_DATAX_TASK_STOPPED_ERROR;
    }
  } /* end while */

  if (foug_datax_no_error(error)
      && geom != NULL && geom->end_triangles_func != NULL)
  {
    geom->end_triangles_func(geom);
  }

  if (foug_datax_no_error(error) && accum_facet_count_read != total_facet_count)
    error = FOUG_STLB_READ_FACET_COUNT_ERROR;
  return error;
}
