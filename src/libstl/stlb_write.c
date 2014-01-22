#include "stlb_write.h"

#include "../endian.h"
#include "../error.h"
#include <string.h>

static void write_triangle_memcpy(const foug_stlb_triangle_t* triangle, uint8_t* buffer)
{
  memcpy(buffer, triangle, FOUG_STLB_TRIANGLE_RAWSIZE);
}

static void write_coords_alignsafe(const foug_stl_coords_t* coords, uint8_t* buffer)
{
  memcpy(buffer, coords, FOUG_STL_COORDS_RAWSIZE);
}

static void write_triangle_alignsafe(const foug_stlb_triangle_t* triangle, uint8_t* buffer)
{
  write_coords_alignsafe(&triangle->data.normal, buffer);
  write_coords_alignsafe(&triangle->data.v1, buffer + 1*FOUG_STL_COORDS_RAWSIZE);
  write_coords_alignsafe(&triangle->data.v2, buffer + 2*FOUG_STL_COORDS_RAWSIZE);
  write_coords_alignsafe(&triangle->data.v3, buffer + 3*FOUG_STL_COORDS_RAWSIZE);
  memcpy(buffer + 4*FOUG_STL_COORDS_RAWSIZE, &triangle->attribute_byte_count, sizeof(uint16_t));
}

static void foug_stlb_write_facets(const foug_stlb_geom_output_t* geom,
                                   uint8_t* buffer,
                                   uint32_t ifacet_start,
                                   uint32_t facet_count)
{
  foug_stlb_triangle_t triangle;
  uint32_t buffer_offset;
  uint32_t i_facet;

  if (geom == NULL || geom->get_triangle_func == NULL)
    return;

  buffer_offset = 0;
  for (i_facet = ifacet_start; i_facet < (ifacet_start + facet_count); ++i_facet) {
    geom->get_triangle_func(geom, i_facet, &triangle);

#ifdef FOUG_STLB_READWRITE_ALIGNSAFE
    write_triangle_alignsafe(&triangle, buffer + buffer_offset);
#else
    write_triangle_memcpy(&triangle, buffer + buffer_offset);
#endif

    buffer_offset += FOUG_STLB_TRIANGLE_RAWSIZE;
  } /* end for */
}

int foug_stlb_write(const foug_stlb_geom_output_t* geom,
                    foug_transfer_t* trsf,
                    foug_endianness_t byte_order)
{
  const uint32_t facet_count = geom != NULL ? geom->triangle_count : 0;
  uint32_t i_facet;
  uint32_t buffer_facet_count;
  uint32_t ifacet_start;
  int error;

  if (trsf->buffer == NULL)
    return FOUG_DATAX_NULL_BUFFER_ERROR;
  if (trsf->buffer_size < FOUG_STLB_MIN_CONTENTS_SIZE)
    return FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;
  if (geom == NULL || geom->get_triangle_func == NULL)
    return FOUG_STLB_WRITE_NULL_GET_TRIANGLE_FUNC;
  if (byte_order != FOUG_LITTLE_ENDIAN/* && byte_order != FOUG_BIG_ENDIAN*/)
    return FOUG_STLB_WRITE_UNSUPPORTED_BYTE_ORDER;

  /* Write header */
  {
    const uint8_t* header_data = NULL;
    if (geom->header != NULL) {
      header_data = geom->header;
    }
    else {
      /* Use buffer to store an empty header (filled with zeroes) */
      memset(trsf->buffer, 0, FOUG_STLB_HEADER_SIZE);
      header_data = (const uint8_t*)trsf->buffer;
    }
    if (foug_stream_write(&trsf->stream, header_data, FOUG_STLB_HEADER_SIZE, 1) != 1)
      return FOUG_DATAX_STREAM_ERROR;
  }

  /* Write facet count */
  if (byte_order == FOUG_LITTLE_ENDIAN)
    foug_encode_uint32_le(facet_count, trsf->buffer);
  else
    foug_encode_uint32_be(facet_count, trsf->buffer);
  if (foug_stream_write(&trsf->stream, trsf->buffer, sizeof(uint32_t), 1) != 1)
    return FOUG_DATAX_STREAM_ERROR;

  /* Write triangles */
  error = FOUG_DATAX_NO_ERROR;

  buffer_facet_count = trsf->buffer_size / FOUG_STLB_TRIANGLE_RAWSIZE;
  ifacet_start = 0;
  for (i_facet = 0;
       i_facet < facet_count && foug_datax_no_error(error);
       i_facet += buffer_facet_count)
  {
    /* Write to buffer */
    if (buffer_facet_count > (facet_count - ifacet_start))
      buffer_facet_count = facet_count - ifacet_start;
    foug_stlb_write_facets(geom, trsf->buffer, ifacet_start, buffer_facet_count);
    ifacet_start += buffer_facet_count;

    /* Write buffer to stream */
    if (foug_stream_write(&trsf->stream, trsf->buffer, FOUG_STLB_TRIANGLE_RAWSIZE, buffer_facet_count)
        != buffer_facet_count)
    {
      error = FOUG_DATAX_STREAM_ERROR;
    }

    /* Task control */
    if (foug_datax_no_error(error)
        && !foug_task_control_handle_progress(&trsf->task_control,
                                              foug_percentage(0, facet_count, i_facet + 1)))
    {
      error = FOUG_DATAX_TASK_STOPPED_ERROR;
    }
  } /* end for */

  return error;
}
