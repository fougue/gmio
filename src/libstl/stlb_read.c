#include "stlb_read.h"

#include "../endian.h"
#include "../error.h"
#include "stl_error.h"

#include "../internal/convert.h"
#include "../internal/byte_swap.h"
#include "../internal/libstl/stlb_byte_swap.h"
#include "../internal/libstl/stlb_rw_common.h"

#include <string.h>

FOUG_INLINE static void read_triangle_memcpy(const uint8_t* buffer,
                                             foug_stlb_triangle_t* triangle)
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
                                  const uint8_t* buffer,
                                  const foug_readwrite_helper* rparams)
{
  const uint32_t facet_count = rparams->facet_count;
  const uint32_t i_facet_offset = rparams->i_facet_offset;
  foug_stlb_triangle_t triangle;
  uint32_t buffer_offset;
  uint32_t i_facet;

  if (geom == NULL || geom->process_triangle_func == NULL)
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

    if (rparams->fix_endian_func != NULL)
      rparams->fix_endian_func(&triangle);

    /* Declare triangle */
    geom->process_triangle_func(geom->cookie,
                                i_facet_offset + i_facet,
                                &triangle.data,
                                triangle.attribute_byte_count);
  }
}

int foug_stlb_read(foug_stlb_geom_input_t* geom,
                   foug_transfer_t* trsf,
                   foug_endianness_t byte_order)
{
  const foug_endianness_t host_byte_order = foug_host_endianness();
  foug_readwrite_helper rparams;
  uint32_t total_facet_count = 0;  /* Count of facets as declared in the stream */
  int error = FOUG_DATAX_NO_ERROR; /* Helper variable to store function result error code  */

  /* Check validity of input parameters */
  error = foug_stlb_check_params(trsf, byte_order);
  if (foug_datax_error(error))
    return error;

  /* Initialize rparams */
  memset(&rparams, 0, sizeof(foug_readwrite_helper));
  if (host_byte_order != byte_order)
    rparams.fix_endian_func = foug_stlb_triangle_bswap;

  /* Read header */
  {
    uint8_t header_data[FOUG_STLB_HEADER_SIZE];
    if (foug_stream_read(&trsf->stream, header_data, 1, FOUG_STLB_HEADER_SIZE)
        != FOUG_STLB_HEADER_SIZE)
    {
      return FOUG_STLB_READ_HEADER_WRONG_SIZE_ERROR;
    }
    if (geom != NULL && geom->process_header_func != NULL)
      geom->process_header_func(geom->cookie, header_data);
  }

  /* Read facet count */
  if (foug_stream_read(&trsf->stream, trsf->buffer, sizeof(uint32_t), 1) != 1)
    return FOUG_STLB_READ_FACET_COUNT_ERROR;

  memcpy(&total_facet_count, trsf->buffer, sizeof(uint32_t));
  if (host_byte_order != byte_order)
    total_facet_count = foug_uint32_bswap(total_facet_count);
  if (geom != NULL && geom->begin_triangles_func != NULL)
    geom->begin_triangles_func(geom->cookie, total_facet_count);

  /* Read triangles */
  while (foug_datax_no_error(error)
         && rparams.i_facet_offset < total_facet_count)
  {
    rparams.facet_count = foug_stream_read(&trsf->stream,
                                               trsf->buffer,
                                               FOUG_STLB_TRIANGLE_RAWSIZE,
                                               trsf->buffer_size / FOUG_STLB_TRIANGLE_RAWSIZE);
    if (foug_stream_error(&trsf->stream) != 0)
      error = FOUG_DATAX_STREAM_ERROR;
    else if (rparams.facet_count > 0)
      error = FOUG_DATAX_NO_ERROR;
    else
      break; /* Exit if no facet to read */

    if (foug_datax_no_error(error)) {
      uint8_t progress_pc;

      foug_stlb_read_facets(geom, trsf->buffer, &rparams);
      rparams.i_facet_offset += rparams.facet_count;
      progress_pc = foug_percentage(0, total_facet_count, rparams.i_facet_offset);
      if (!foug_task_control_handle_progress(&trsf->task_control, progress_pc))
        error = FOUG_DATAX_TASK_STOPPED_ERROR;
    }
  } /* end while */

  if (foug_datax_no_error(error)
      && geom != NULL && geom->end_triangles_func != NULL)
  {
    geom->end_triangles_func(geom->cookie);
  }

  if (foug_datax_no_error(error) && rparams.i_facet_offset != total_facet_count)
    error = FOUG_STLB_READ_FACET_COUNT_ERROR;
  return error;
}
