/****************************************************************************
**
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
**
****************************************************************************/

#include "stl_io.h"

#include "stl_error.h"
#include "internal/stl_rw_common.h"
#include "internal/stlb_byte_swap.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/error.h"
#include "../gmio_core/internal/convert.h"
#include "../gmio_core/internal/byte_swap.h"

#include <string.h>

GMIO_INLINE static void read_triangle_memcpy(const uint8_t* buffer,
                                             gmio_stl_triangle_t* triangle)
{
  /* *triangle = *((gmio_stl_triangle_t*)(buffer)); */
  memcpy(triangle, buffer, GMIO_STLB_TRIANGLE_RAWSIZE);
}

static void gmio_stlb_read_facets(gmio_stl_mesh_creator_t* creator,
                                  const uint8_t* buffer,
                                  const gmio_stlb_readwrite_helper_t* rparams)
{
  const uint32_t facet_count = rparams->facet_count;
  const uint32_t i_facet_offset = rparams->i_facet_offset;
  gmio_stl_triangle_t triangle;
  uint32_t buffer_offset = 0;
  uint32_t i_facet = 0;

  if (creator == NULL || creator->add_triangle_func == NULL)
    return;

  for (i_facet = 0; i_facet < facet_count; ++i_facet) {
    /* Decode data */
    read_triangle_memcpy(buffer + buffer_offset, &triangle);
    buffer_offset += GMIO_STLB_TRIANGLE_RAWSIZE;

    if (rparams->fix_endian_func != NULL)
      rparams->fix_endian_func(&triangle);

    /* Declare triangle */
    creator->add_triangle_func(creator->cookie, i_facet_offset + i_facet, &triangle);
  }
}

int gmio_stlb_read(gmio_stl_mesh_creator_t *creator,
                   gmio_transfer_t* trsf,
                   const gmio_stlb_read_options_t* options)
{
  const gmio_endianness_t host_byte_order = gmio_host_endianness();
  const gmio_endianness_t byte_order = options != NULL ? options->byte_order : host_byte_order;
  gmio_stlb_readwrite_helper_t rparams = {0};
  uint8_t  header_data[GMIO_STLB_HEADER_SIZE];
  uint32_t total_facet_count = 0;  /* Count of facets as declared in the stream */
  int error = GMIO_NO_ERROR; /* Helper variable to store function result error code  */

  /* Check validity of input parameters */
  if (!gmio_stlb_check_params(&error, trsf, byte_order))
    return error;

  /* Initialize rparams */
  if (host_byte_order != byte_order)
    rparams.fix_endian_func = gmio_stl_triangle_bswap;

  /* Read header */
  if (gmio_stream_read(&trsf->stream, header_data, 1, GMIO_STLB_HEADER_SIZE)
      != GMIO_STLB_HEADER_SIZE)
  {
    return GMIO_STLB_READ_HEADER_WRONG_SIZE_ERROR;
  }

  /* Read facet count */
  if (gmio_stream_read(&trsf->stream, trsf->buffer, sizeof(uint32_t), 1) != 1)
    return GMIO_STLB_READ_FACET_COUNT_ERROR;

  memcpy(&total_facet_count, trsf->buffer, sizeof(uint32_t));
  if (host_byte_order != byte_order)
    total_facet_count = gmio_uint32_bswap(total_facet_count);

  /* Callback to notify triangle count and header data */
  if (creator != NULL && creator->binary_begin_solid_func != NULL)
    creator->binary_begin_solid_func(creator->cookie, total_facet_count, header_data);

  /* Read triangles */
  while (gmio_no_error(error)
         && rparams.i_facet_offset < total_facet_count)
  {
    rparams.facet_count = gmio_stream_read(&trsf->stream,
                                           trsf->buffer,
                                           GMIO_STLB_TRIANGLE_RAWSIZE,
                                           trsf->buffer_size / GMIO_STLB_TRIANGLE_RAWSIZE);
    if (gmio_stream_error(&trsf->stream) != 0)
      error = GMIO_STREAM_ERROR;
    else if (rparams.facet_count > 0)
      error = GMIO_NO_ERROR;
    else
      break; /* Exit if no facet to read */

    if (gmio_no_error(error)) {
      gmio_stlb_read_facets(creator, trsf->buffer, &rparams);
      rparams.i_facet_offset += rparams.facet_count;
      if (gmio_task_control_is_stop_requested(&trsf->task_control))
        error = GMIO_TASK_STOPPED_ERROR;
    }
  } /* end while */

  if (gmio_no_error(error)
      && creator != NULL
      && creator->end_solid_func != NULL)
  {
    creator->end_solid_func(creator->cookie);
  }

  if (gmio_no_error(error) && rparams.i_facet_offset != total_facet_count)
    error = GMIO_STLB_READ_FACET_COUNT_ERROR;
  return error;
}
