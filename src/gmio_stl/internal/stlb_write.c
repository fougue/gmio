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

GMIO_INLINE void write_triangle_memcpy(
        const struct gmio_stl_triangle* triangle, uint8_t* mblock)
{
    memcpy(mblock, triangle, GMIO_STLB_TRIANGLE_RAWSIZE);
}

static void gmio_stlb_write_facets(
        const struct gmio_stl_mesh* mesh,
        uint8_t* mblock,
        const struct gmio_stlb_readwrite_helper* wparams)
{
    const uint32_t facet_count = wparams->facet_count;
    const uint32_t i_facet_offset = wparams->i_facet_offset;
    const gmio_stl_triangle_func_fix_endian_t func_fix_endian =
            wparams->func_fix_endian;
    const gmio_stl_mesh_func_get_triangle_t func_get_triangle =
            mesh->func_get_triangle;
    const void* cookie = mesh->cookie;
    struct gmio_stl_triangle triangle;
    uint32_t mblock_offset = 0;
    uint32_t i_facet = 0;

    triangle.attribute_byte_count = 0;
    for (i_facet = 0; i_facet < facet_count; ++i_facet) {
        func_get_triangle(cookie, i_facet_offset + i_facet, &triangle);

        if (func_fix_endian != NULL)
            func_fix_endian(&triangle);

        write_triangle_memcpy(&triangle, mblock + mblock_offset);
        mblock_offset += GMIO_STLB_TRIANGLE_RAWSIZE;
    } /* end for */
}

int gmio_stlb_write(
        struct gmio_stl_write_args* args, enum gmio_endianness byte_order)
{
    /* Constants */
    const uint32_t facet_count =
            args->mesh.triangle_count;
    const gmio_bool_t write_triangles_only =
            args->options.stl_write_triangles_only;
    /* Variables */
    struct gmio_rwargs* core_args = &args->core;
    void* mblock_ptr = core_args->memblock.ptr;
    struct gmio_stlb_readwrite_helper wparams = {0};
    uint32_t i_facet = 0;
    int error = GMIO_ERROR_OK;

    /* Check validity of input parameters */
    if (!gmio_stl_check_mesh(&error, &args->mesh)
            || !gmio_stlb_check_params(&error, core_args, byte_order))
    {
        return error;
    }

    /* Initialize wparams */
    if (byte_order != GMIO_ENDIANNESS_HOST)
        wparams.func_fix_endian = gmio_stl_triangle_bswap;
    /* Note: trsf != NULL  certified by gmio_stlb_check_params() */
    /* coverity[var_deref_op : FALSE] */
    wparams.facet_count = gmio_size_to_uint32(
                core_args->memblock.size / GMIO_STLB_TRIANGLE_RAWSIZE);

    if (!write_triangles_only) {
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
         i_facet += wparams.facet_count)
    {
        gmio_rwargs_handle_progress(core_args, i_facet, facet_count);

        /* Write to memory block */
        wparams.facet_count = GMIO_MIN(wparams.facet_count,
                                       facet_count - wparams.i_facet_offset);

        gmio_stlb_write_facets(&args->mesh, mblock_ptr, &wparams);
        wparams.i_facet_offset += wparams.facet_count;

        /* Write memory block to stream */
        if (gmio_stream_write(
                    &core_args->stream,
                    mblock_ptr,
                    GMIO_STLB_TRIANGLE_RAWSIZE,
                    wparams.facet_count)
                != wparams.facet_count)
        {
            error = GMIO_ERROR_STREAM;
        }

        /* Handle stop request */
        if (gmio_no_error(error) && gmio_rwargs_is_stop_requested(core_args))
            error = GMIO_ERROR_TRANSFER_STOPPED;
    } /* end for */

    return error;
}
