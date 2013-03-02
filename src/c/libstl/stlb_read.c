#include "stlb_read.h"

#include "../endian.h"

static foug_bool_t foug_stlb_no_error(int code)
{
  return code == FOUG_STLB_READ_NO_ERROR;
}

static void foug_stlb_read_facets(foug_stlb_geom_input_t* geom_input,
                                  uint8_t* buffer,
                                  uint32_t facet_count)
{
  foug_stlb_triangle_t triangle;
  uint32_t buffer_offset;
  uint32_t i_facet;

  if (geom_input->process_next_triangle_func == NULL)
    return;

  buffer_offset = 0;
  for (i_facet = 0; i_facet < facet_count; ++i_facet) {
    /* Decode data */
    triangle = *((foug_stlb_triangle_t*)(buffer + buffer_offset));
    buffer_offset += FOUG_STLB_TRIANGLE_SIZE;

    /* Declare triangle */
    geom_input->process_next_triangle_func(geom_input, &triangle);
  }
}

int foug_stlb_read(foug_stlb_read_args_t *args)
{
  foug_task_progress_t progress;
  uint8_t header_data[FOUG_STLB_HEADER_SIZE];
  uint32_t total_facet_count;
  size_t buffer_facet_count;
  size_t accum_facet_count_read;
  int error;
  size_t facet_count_read;

  if (args->buffer == NULL)
    return FOUG_STLB_READ_NULL_BUFFER;
  if (args->buffer_size < FOUG_STLB_MIN_CONTENTS_SIZE)
    return FOUG_STLB_READ_INVALID_BUFFER_SIZE_ERROR;

  /* Read header */
  if (foug_stream_read(&args->stream, header_data, 1, FOUG_STLB_HEADER_SIZE) != FOUG_STLB_HEADER_SIZE)
    return FOUG_STLB_READ_HEADER_WRONG_SIZE_ERROR;

  if (args->geom_input.process_header_func != NULL)
    args->geom_input.process_header_func(&args->geom_input, header_data);

  /* Read facet count */
  if (foug_stream_read(&args->stream, args->buffer, sizeof(uint32_t), 1) != 1)
    return FOUG_STLB_READ_FACET_COUNT_ERROR;

  total_facet_count = foug_decode_uint32_le(args->buffer);
  if (args->geom_input.begin_triangles_func != NULL)
    args->geom_input.begin_triangles_func(&args->geom_input, total_facet_count);

  progress.range_min = 0.f;
  progress.range_max = (foug_real32_t)total_facet_count;
  progress.value = 0.f;
  args->task_control.is_stop_requested = 0;

  /* Read triangles */
  buffer_facet_count = args->buffer_size / FOUG_STLB_TRIANGLE_SIZE;
  accum_facet_count_read = 0;
  error = FOUG_STLB_READ_NO_ERROR;
  while (foug_stlb_no_error(error) && accum_facet_count_read < total_facet_count) {
    facet_count_read = foug_stream_read(&args->stream,
                                        args->buffer, FOUG_STLB_TRIANGLE_SIZE, buffer_facet_count);
    if (foug_stream_error(&args->stream) != 0)
      error = FOUG_STLB_READ_STREAM_ERROR;
    else if (facet_count_read > 0)
      error = FOUG_STLB_READ_NO_ERROR;
    else
      break; /* Exit if no facet to read */

    if (foug_stlb_no_error(error)) {
      foug_stlb_read_facets(&args->geom_input, args->buffer, facet_count_read);
      accum_facet_count_read += facet_count_read;
      if (args->task_control.is_stop_requested) {
        error = FOUG_STLB_READ_TASK_STOPPED_ERROR;
        foug_task_control_handle_stop(&args->task_control);
      }
      else {
        foug_task_control_set_progress(&args->task_control, &progress, accum_facet_count_read);
      }
    }
  } /* end while */

  if (foug_stlb_no_error(error) && args->geom_input.end_triangles_func != NULL)
    args->geom_input.end_triangles_func(&args->geom_input);

  if (foug_stlb_no_error(error) && accum_facet_count_read != total_facet_count)
    error = FOUG_STLB_READ_FACET_COUNT_ERROR;
  return error;
}
