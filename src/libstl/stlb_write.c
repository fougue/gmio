#include "stlb_write.h"

#include "../endian.h"
#include "../error.h"
#include "stl_error.h"

#include "../internal/byte_codec.h"
#include "../internal/libstl/stlb_byte_swap.h"
#include "../internal/libstl/stlb_rw_common.h"

#include <string.h>

FOUG_INLINE static void write_triangle_memcpy(const foug_stlb_triangle_t* triangle,
                                              uint8_t* buffer)
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
                                   const foug_readwrite_helper* wparams)
{
  const uint32_t facet_count = wparams->facet_count;
  const uint32_t i_facet_offset = wparams->i_facet_offset;
  foug_stlb_triangle_t triangle;
  uint32_t buffer_offset = 0;
  uint32_t i_facet = 0;

  if (geom == NULL || geom->get_triangle_func == NULL)
    return;

  triangle.attribute_byte_count = 0;
  for (i_facet = i_facet_offset; i_facet < (i_facet_offset + facet_count); ++i_facet) {
    geom->get_triangle_func(geom->cookie, i_facet, &triangle.data);
    if (geom->get_attr_byte_count_func != NULL)
      geom->get_attr_byte_count_func(geom->cookie, i_facet, &triangle.attribute_byte_count);

    if (wparams->fix_endian_func != NULL)
      wparams->fix_endian_func(&triangle);

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
  foug_readwrite_helper wparams;
  const uint32_t facet_count = geom != NULL ? geom->triangle_count : 0;
  uint32_t i_facet = 0;
  int error = FOUG_DATAX_NO_ERROR;

  /* Check validity of input parameters */
  error = foug_stlb_check_params(trsf, byte_order);
  if (foug_datax_error(error))
    return error;
  if (geom == NULL || geom->get_triangle_func == NULL)
    return FOUG_STLB_WRITE_NULL_GET_TRIANGLE_FUNC;

  /* Initialize wparams */
  memset(&wparams, 0, sizeof(foug_readwrite_helper));
  if (foug_host_endianness() != byte_order)
    wparams.fix_endian_func = foug_stlb_triangle_bswap;
  wparams.facet_count = trsf->buffer_size / FOUG_STLB_TRIANGLE_RAWSIZE;

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
  for (i_facet = 0;
       i_facet < facet_count && foug_datax_no_error(error);
       i_facet += wparams.facet_count)
  {
    /* Write to buffer */
    if (wparams.facet_count > (facet_count - wparams.i_facet_offset))
      wparams.facet_count = facet_count - wparams.i_facet_offset;

    foug_stlb_write_facets(geom, trsf->buffer, &wparams);
    wparams.i_facet_offset += wparams.facet_count;

    /* Write buffer to stream */
    if (foug_stream_write(&trsf->stream, trsf->buffer, FOUG_STLB_TRIANGLE_RAWSIZE, wparams.facet_count)
        != wparams.facet_count)
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
