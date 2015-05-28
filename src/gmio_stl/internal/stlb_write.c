/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougsys.fr
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

#include "stlb_write.h"

#include "stl_rw_common.h"
#include "stlb_byte_swap.h"
#include "../stl_error.h"
#include "../stl_io.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/internal/byte_codec.h"
#include "../../gmio_core/internal/min_max.h"
#include "../../gmio_core/internal/helper_stream.h"
#include "../../gmio_core/internal/helper_transfer.h"
#include "../../gmio_core/internal/safe_cast.h"

#include <string.h>

GMIO_INLINE void write_triangle_memcpy(
        const gmio_stl_triangle_t* triangle, uint8_t* buffer)
{
    memcpy(buffer, triangle, GMIO_STLB_TRIANGLE_RAWSIZE);
}

static void gmio_stlb_write_facets(
        const gmio_stl_mesh_t* mesh,
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
    for (i_facet = i_facet_offset;
         i_facet < (i_facet_offset + facet_count);
         ++i_facet)
    {
        mesh->get_triangle_func(mesh->cookie, i_facet, &triangle);

        if (wparams->fix_endian_func != NULL)
            wparams->fix_endian_func(&triangle);

        write_triangle_memcpy(&triangle, buffer + buffer_offset);

        buffer_offset += GMIO_STLB_TRIANGLE_RAWSIZE;
    } /* end for */
}

int gmio_stlb_write(
        gmio_transfer_t* trsf,
        const gmio_stl_mesh_t* mesh,
        const gmio_stl_write_options_t* options,
        gmio_endianness_t byte_order)
{
    /* Constants */
    const uint32_t facet_count =
            mesh != NULL ? mesh->triangle_count : 0;
    const gmio_bool_t write_triangles_only =
            options != NULL ? options->stl_write_triangles_only : GMIO_FALSE;
    /* Variables */
    void* buffer_ptr = trsf != NULL ? trsf->buffer.ptr : NULL;
    gmio_stlb_readwrite_helper_t wparams = {0};
    uint32_t i_facet = 0;
    int error = GMIO_ERROR_OK;

    /* Check validity of input parameters */
    gmio_stl_check_mesh(&error, mesh);
    gmio_stlb_check_params(&error, trsf, byte_order);
    if (gmio_error(error))
        return error;

    /* Initialize wparams */
    if (byte_order != GMIO_ENDIANNESS_HOST)
        wparams.fix_endian_func = gmio_stl_triangle_bswap;
    wparams.facet_count = gmio_size_to_uint32(
                trsf->buffer.size / GMIO_STLB_TRIANGLE_RAWSIZE);

    if (!write_triangles_only) {
        error = gmio_stlb_write_header(
                    &trsf->stream,
                    byte_order,
                    options != NULL ? options->stlb_header_data : NULL,
                    facet_count);
        if (gmio_error(error))
            return error;
    }

    /* Write triangles */
    for (i_facet = 0;
         i_facet < facet_count && gmio_no_error(error);
         i_facet += wparams.facet_count)
    {
        gmio_transfer_handle_progress(trsf, i_facet, facet_count);

        /* Write to buffer */
        wparams.facet_count = GMIO_MIN(wparams.facet_count,
                                       facet_count - wparams.i_facet_offset);

        gmio_stlb_write_facets(mesh, buffer_ptr, &wparams);
        wparams.i_facet_offset += wparams.facet_count;

        /* Write buffer to stream */
        if (gmio_stream_write(
                    &trsf->stream,
                    buffer_ptr,
                    GMIO_STLB_TRIANGLE_RAWSIZE,
                    wparams.facet_count)
                != wparams.facet_count)
        {
            error = GMIO_ERROR_STREAM;
        }

        /* Handle stop request */
        if (gmio_no_error(error) && gmio_transfer_is_stop_requested(trsf))
            error = GMIO_ERROR_TRANSFER_STOPPED;
    } /* end for */

    return error;
}
