#include "stlb_read.h"

#include "../convert.h"
#include "../error.h"
#include "../endian.h"

#include <string.h>

static void read_triangle_memcpy(const uint8_t* buffer, foug_stlb_triangle_t* triangle)
{
  /* *triangle = *((foug_stlb_triangle_t*)(buffer)); */
  memcpy(triangle, buffer, FOUG_STLB_TRIANGLE_RAWSIZE);
}

static void read_coords_alignsafe(const uint8_t* buffer, foug_stl_coords_t* coords)
{
  memcpy(coords, buffer, FOUG_STL_COORDS_RAWSIZE);
}

static void read_triangle_alignsafe(const uint8_t* buffer, foug_stlb_triangle_t* triangle)
{
  read_coords_alignsafe(buffer, &triangle->data.normal);
  read_coords_alignsafe(buffer + 1*FOUG_STL_COORDS_RAWSIZE, &triangle->data.v1);
  read_coords_alignsafe(buffer + 2*FOUG_STL_COORDS_RAWSIZE, &triangle->data.v2);
  read_coords_alignsafe(buffer + 3*FOUG_STL_COORDS_RAWSIZE, &triangle->data.v3);
  memcpy(&triangle->attribute_byte_count, buffer + 4*FOUG_STL_COORDS_RAWSIZE, sizeof(uint16_t));
}

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
#ifdef FOUG_STLB_READWRITE_ALIGNSAFE
    read_triangle_alignsafe(buffer + buffer_offset, &triangle);
#else
    read_triangle_memcpy(buffer + buffer_offset, &triangle);
#endif
    buffer_offset += FOUG_STLB_TRIANGLE_RAWSIZE;

    /* Declare triangle */
    geom->process_next_triangle_func(geom, &triangle);
  }
}

int foug_stlb_read(foug_stlb_geom_input_t* geom,
                   foug_transfer_t* trsf,
                   foug_endianness_t byte_order)
{
  uint32_t total_facet_count;
  size_t accum_facet_count_read;
  int error;

  if (trsf->buffer == NULL)
    return FOUG_DATAX_NULL_BUFFER_ERROR;
  if (trsf->buffer_size < FOUG_STLB_MIN_CONTENTS_SIZE)
    return FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;
  if (byte_order != FOUG_LITTLE_ENDIAN)
    return FOUG_STLB_READ_UNSUPPORTED_BYTE_ORDER;

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
                                                     FOUG_STLB_TRIANGLE_RAWSIZE,
                                                     trsf->buffer_size / FOUG_STLB_TRIANGLE_RAWSIZE);
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
