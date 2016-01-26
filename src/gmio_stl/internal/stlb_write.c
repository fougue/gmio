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

#include "stlb_write.h"

#include "stl_funptr_typedefs.h"
#include "stl_rw_common.h"
#include "stlb_byte_swap.h"
#include "../stl_error.h"
#include "../stl_io.h"
#include "../stl_io_options.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/internal/byte_codec.h"
#include "../../gmio_core/internal/min_max.h"
#include "../../gmio_core/internal/helper_rwargs.h"
#include "../../gmio_core/internal/helper_stream.h"
#include "../../gmio_core/internal/safe_cast.h"

#include <string.h>

GMIO_INLINE void encode_facet(
        const struct gmio_stl_triangle* triangle, uint8_t* buffer)
{
    memcpy(buffer, triangle, GMIO_STLB_TRIANGLE_RAWSIZE);
}

typedef void (*func_gmio_stlb_encode_facets_t)(
        const struct gmio_stl_mesh*,
        uint8_t*,        /* buffer */
        const uint32_t,  /* facet_count */
        const uint32_t); /* i_facet_offset */

static void gmio_stlb_encode_facets(
        const struct gmio_stl_mesh* mesh,
        uint8_t* buffer,
        const uint32_t facet_count,
        const uint32_t i_facet_offset)
{
    const gmio_stl_mesh_func_get_triangle_t func_get_triangle =
            mesh->func_get_triangle;
    const void* cookie = mesh->cookie;
    struct gmio_stl_triangle triangle;
    uint32_t i_facet;

    triangle.attribute_byte_count = 0;
    for (i_facet = 0; i_facet < facet_count; ++i_facet) {
        func_get_triangle(cookie, i_facet_offset + i_facet, &triangle);
        encode_facet(&triangle, buffer);
        buffer += GMIO_STLB_TRIANGLE_RAWSIZE;
    }
}

static void gmio_stlb_encode_facets_byteswap(
        const struct gmio_stl_mesh* mesh,
        uint8_t* buffer,
        const uint32_t facet_count,
        const uint32_t i_facet_offset)
{
    const gmio_stl_mesh_func_get_triangle_t func_get_triangle =
            mesh->func_get_triangle;
    const void* cookie = mesh->cookie;
    struct gmio_stl_triangle triangle;
    uint32_t i_facet;

    triangle.attribute_byte_count = 0;
    for (i_facet = 0; i_facet < facet_count; ++i_facet) {
        func_get_triangle(cookie, i_facet_offset + i_facet, &triangle);
        gmio_stl_triangle_bswap(&triangle);
        encode_facet(&triangle, buffer);
        buffer += GMIO_STLB_TRIANGLE_RAWSIZE;
    }
}

int gmio_stlb_write(
        struct gmio_stl_write_args* args, enum gmio_endianness byte_order)
{
    /* Constants */
    const uint32_t facet_count = args->mesh.triangle_count;
    const func_gmio_stlb_encode_facets_t func_encode_facets =
            byte_order != GMIO_ENDIANNESS_HOST ?
                gmio_stlb_encode_facets_byteswap :
                gmio_stlb_encode_facets;

    /* Variables */
    struct gmio_rwargs* core_args = &args->core;
    uint32_t i_facet = 0; /* Facet counter */
    uint32_t write_facet_count =
            gmio_size_to_uint32(
                core_args->stream_memblock.size / GMIO_STLB_TRIANGLE_RAWSIZE);
    void* mblock_ptr = core_args->stream_memblock.ptr;
    int error = GMIO_ERROR_OK;

    /* Check validity of input parameters */
    if (!gmio_stl_check_mesh(&error, &args->mesh)
            || !gmio_stlb_check_params(&error, core_args, byte_order))
    {
        return error;
    }

    if (!args->options.stl_write_triangles_only) {
        error = gmio_stlb_write_header(
                    &core_args->stream,
                    byte_order,
                    args->options.stlb_header,
                    facet_count);
        if (gmio_error(error))
            return error;
    }

    /* Write triangles */
    for (i_facet = 0;
         i_facet < facet_count && gmio_no_error(error);
         i_facet += write_facet_count)
    {
        gmio_rwargs_handle_progress(core_args, i_facet, facet_count);

        /* Write to memory block */
        write_facet_count = GMIO_MIN(write_facet_count, facet_count - i_facet);
        func_encode_facets(
                    &args->mesh, mblock_ptr, write_facet_count, i_facet);

        /* Write memory block to stream */
        if (gmio_stream_write(
                    &core_args->stream,
                    mblock_ptr,
                    GMIO_STLB_TRIANGLE_RAWSIZE,
                    write_facet_count)
                != write_facet_count)
        {
            error = GMIO_ERROR_STREAM;
        }

        /* Handle stop request */
        if (gmio_no_error(error) && gmio_rwargs_is_stop_requested(core_args))
            error = GMIO_ERROR_TRANSFER_STOPPED;
    } /* end for */

    return error;
}
