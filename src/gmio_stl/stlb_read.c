/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include "stl_io.h"

#include "stl_error.h"
#include "internal/helper_stl_mesh_creator.h"
#include "internal/stl_rw_common.h"
#include "internal/stlb_byte_swap.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/error.h"
#include "../gmio_core/internal/byte_swap.h"
#include "../gmio_core/internal/convert.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/helper_transfer.h"
#include "../gmio_core/internal/safe_cast.h"

#include <string.h>

GMIO_INLINE void read_triangle_memcpy(
        const uint8_t* buffer, gmio_stl_triangle_t* triangle)
{
    /* *triangle = *((gmio_stl_triangle_t*)(buffer)); */
    memcpy(triangle, buffer, GMIO_STLB_TRIANGLE_RAWSIZE);
}

static void gmio_stlb_read_facets(
        gmio_stl_mesh_creator_t* creator,
        const uint8_t* buffer,
        const gmio_stlb_readwrite_helper_t* rparams)
{
    const uint32_t facet_count = rparams->facet_count;
    const uint32_t i_facet_offset = rparams->i_facet_offset;
    gmio_stl_triangle_t triangle;
    uint32_t buffer_offset = 0;
    uint32_t i_facet = 0;

    if (creator == NULL || creator->func_add_triangle == NULL)
        return;

    for (i_facet = 0; i_facet < facet_count; ++i_facet) {
        /* Decode data */
        read_triangle_memcpy(buffer + buffer_offset, &triangle);
        buffer_offset += GMIO_STLB_TRIANGLE_RAWSIZE;

        if (rparams->func_fix_endian != NULL)
            rparams->func_fix_endian(&triangle);

        /* Declare triangle */
        creator->func_add_triangle(
                    creator->cookie, i_facet_offset + i_facet, &triangle);
    }
}

int gmio_stlb_read(
        gmio_transfer_t* trsf,
        gmio_stl_mesh_creator_t *creator,
        gmio_endianness_t byte_order)
{
    /* Constants */
    const uint32_t max_facet_count_per_read =
            trsf != NULL ?
                gmio_size_to_uint32(
                    trsf->memblock.size / GMIO_STLB_TRIANGLE_RAWSIZE)
              : 0;
    /* Variables */
    void* mblock_ptr = trsf != NULL ? trsf->memblock.ptr : NULL;
    gmio_stlb_readwrite_helper_t rparams = {0};
    gmio_stlb_header_t header;
    uint32_t total_facet_count = 0; /* Facet count, as declared in the stream */
    int error = GMIO_ERROR_OK; /* Helper  to store function result error code */

    /* Check validity of input parameters */
    if (!gmio_stlb_check_params(&error, trsf, byte_order))
        return error;

    /* Initialize rparams */
    if (byte_order != GMIO_ENDIANNESS_HOST)
        rparams.func_fix_endian = gmio_stl_triangle_bswap;

    /* Read header */
    if (gmio_stream_read(&trsf->stream, &header, GMIO_STLB_HEADER_SIZE, 1)
            != 1)
    {
        return GMIO_STL_ERROR_HEADER_WRONG_SIZE;
    }

    /* Read facet count */
    if (gmio_stream_read(&trsf->stream, mblock_ptr, sizeof(uint32_t), 1)
            != 1)
    {
        return GMIO_STL_ERROR_FACET_COUNT;
    }

    memcpy(&total_facet_count, mblock_ptr, sizeof(uint32_t));
    if (byte_order != GMIO_ENDIANNESS_HOST)
        total_facet_count = gmio_uint32_bswap(total_facet_count);

    /* Callback to notify triangle count and header data */
    gmio_stl_mesh_creator_binary_begin_solid(
                creator, total_facet_count, &header);

    /* Read triangles */
    while (gmio_no_error(error)
           && rparams.i_facet_offset < total_facet_count)
    {
        gmio_transfer_handle_progress(
                    trsf, rparams.i_facet_offset, total_facet_count);

        rparams.facet_count =
                gmio_size_to_uint32(
                    gmio_stream_read(
                        &trsf->stream,
                        mblock_ptr,
                        GMIO_STLB_TRIANGLE_RAWSIZE,
                        max_facet_count_per_read));
        if (gmio_stream_error(&trsf->stream) != 0)
            error = GMIO_ERROR_STREAM;
        else if (rparams.facet_count > 0)
            error = GMIO_ERROR_OK;
        else
            break; /* Exit if no facet to read */

        if (gmio_no_error(error)) {
            gmio_stlb_read_facets(creator, mblock_ptr, &rparams);
            rparams.i_facet_offset += rparams.facet_count;
            if (gmio_transfer_is_stop_requested(trsf))
                error = GMIO_ERROR_TRANSFER_STOPPED;
        }
    } /* end while */

    if (gmio_no_error(error))
        gmio_stl_mesh_creator_end_solid(creator);

    if (gmio_no_error(error) && rparams.i_facet_offset != total_facet_count)
        error = GMIO_STL_ERROR_FACET_COUNT;
    return error;
}
