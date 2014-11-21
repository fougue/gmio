#include "stl_io.h"

#include "stl_error.h"
#include "internal/stl_rw_common.h"
#include "internal/stlb_byte_swap.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/error.h"
#include "../gmio_core/internal/byte_codec.h"

#include <string.h>

GMIO_INLINE static void write_triangle_memcpy(const gmio_stl_triangle_t* triangle,
                                              uint8_t* buffer)
{
  memcpy(buffer, triangle, GMIO_STLB_TRIANGLE_RAWSIZE);
}

static void gmio_stlb_write_facets(const gmio_stl_mesh_t* mesh,
                                   uint8_t* buffer,
                                   const gmio_stlb_readwrite_helper_t* wparams)
{
  const uint32_t facet_count = wparams->facet_count;
  const uint32_t i_facet_offset = wparams->i_facet_offset;
  gmio_stl_triangle_t triangle;
  uint32_t buffer_offset = 0;
  uint32_t i_facet = 0;

  if (mesh == NULL || mesh->get_triangle_func == NULL)
    return;

  triangle.attribute_byte_count = 0;
  for (i_facet = i_facet_offset; i_facet < (i_facet_offset + facet_count); ++i_facet) {
    mesh->get_triangle_func(mesh->cookie, i_facet, &triangle);

    if (wparams->fix_endian_func != NULL)
      wparams->fix_endian_func(&triangle);

    write_triangle_memcpy(&triangle, buffer + buffer_offset);

    buffer_offset += GMIO_STLB_TRIANGLE_RAWSIZE;
  } /* end for */
}

int gmio_stlb_write(const gmio_stl_mesh_t* mesh,
                    gmio_transfer_t* trsf,
                    const gmio_stlb_write_options_t* options)
{
  const gmio_endianness_t host_byte_order = gmio_host_endianness();
  const gmio_endianness_t byte_order = options != NULL ? options->byte_order : host_byte_order;
  const uint8_t* header_data = options != NULL ? options->header_data : NULL;
  gmio_stlb_readwrite_helper_t wparams = {0};
  const uint32_t facet_count = mesh != NULL ? mesh->triangle_count : 0;
  uint32_t i_facet = 0;
  int error = GMIO_NO_ERROR;

  /* Check validity of input parameters */
  gmio_stl_check_mesh(&error, mesh);
  gmio_stlb_check_params(&error, trsf, byte_order);
  if (gmio_error(error))
    return error;

  /* Initialize wparams */
  if (host_byte_order != byte_order)
    wparams.fix_endian_func = gmio_stl_triangle_bswap;
  wparams.facet_count = trsf->buffer_size / GMIO_STLB_TRIANGLE_RAWSIZE;

  /* Write header */
  if (header_data == NULL) {
    /* Use buffer to store an empty header (filled with zeroes) */
    memset(trsf->buffer, 0, GMIO_STLB_HEADER_SIZE);
    header_data = (const uint8_t*)trsf->buffer;
  }
  if (gmio_stream_write(&trsf->stream, header_data, GMIO_STLB_HEADER_SIZE, 1) != 1)
    return GMIO_STREAM_ERROR;

  /* Write facet count */
  if (byte_order == GMIO_LITTLE_ENDIAN)
    gmio_encode_uint32_le(facet_count, trsf->buffer);
  else
    gmio_encode_uint32_be(facet_count, trsf->buffer);
  if (gmio_stream_write(&trsf->stream, trsf->buffer, sizeof(uint32_t), 1) != 1)
    return GMIO_STREAM_ERROR;

  /* Write triangles */
  for (i_facet = 0;
       i_facet < facet_count && gmio_no_error(error);
       i_facet += wparams.facet_count)
  {
    /* Write to buffer */
    if (wparams.facet_count > (facet_count - wparams.i_facet_offset))
      wparams.facet_count = facet_count - wparams.i_facet_offset;

    gmio_stlb_write_facets(mesh, trsf->buffer, &wparams);
    wparams.i_facet_offset += wparams.facet_count;

    /* Write buffer to stream */
    if (gmio_stream_write(&trsf->stream, trsf->buffer, GMIO_STLB_TRIANGLE_RAWSIZE, wparams.facet_count)
        != wparams.facet_count)
    {
      error = GMIO_STREAM_ERROR;
    }

    /* Task control */
    if (gmio_no_error(error) && gmio_task_control_is_stop_requested(&trsf->task_control))
      error = GMIO_TASK_STOPPED_ERROR;
  } /* end for */

  return error;
}
