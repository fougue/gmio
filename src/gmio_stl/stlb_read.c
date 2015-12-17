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
#include "internal/stl_funptr_typedefs.h"
#include "internal/stl_rw_common.h"
#include "internal/stlb_byte_swap.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/error.h"
#include "../gmio_core/internal/byte_swap.h"
#include "../gmio_core/internal/convert.h"
#include "../gmio_core/internal/helper_memblock.h"
#include "../gmio_core/internal/helper_rwargs.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/safe_cast.h"

#include <string.h>

GMIO_INLINE void read_triangle_memcpy(
        const uint8_t* buffer, struct gmio_stl_triangle* triangle)
{
    /* *triangle = *((struct gmio_stl_triangle*)(buffer)); */
    memcpy(triangle, buffer, GMIO_STLB_TRIANGLE_RAWSIZE);
}

static void gmio_stlb_read_facets(
        struct gmio_stl_mesh_creator* creator,
        const uint8_t* buffer,
        const struct gmio_stlb_readwrite_helper* rparams)
{
    const uint32_t facet_count = rparams->facet_count;
    const uint32_t i_facet_offset = rparams->i_facet_offset;
    const gmio_stl_triangle_func_fix_endian_t func_fix_endian =
            rparams->func_fix_endian;
    /* coverity[var_deref_op : FALSE] */
    const gmio_stl_mesh_creator_func_add_triangle_t func_add_triangle =
            creator != NULL ? creator->func_add_triangle : NULL;
    void* cookie = creator->cookie;
    struct gmio_stl_triangle triangle;
    uint32_t buffer_offset = 0;
    uint32_t i_facet = 0;

    if (func_add_triangle == NULL)
        return;

    for (i_facet = 0; i_facet < facet_count; ++i_facet) {
        read_triangle_memcpy(buffer + buffer_offset, &triangle);
        buffer_offset += GMIO_STLB_TRIANGLE_RAWSIZE;

        if (func_fix_endian != NULL)
            func_fix_endian(&triangle);

        func_add_triangle(cookie, i_facet_offset + i_facet, &triangle);
    }
}

int gmio_stlb_read(
        struct gmio_stl_read_args* args, enum gmio_endianness byte_order)
{
    /* Variables */
    struct gmio_memblock_helper mblock_helper =
            gmio_memblock_helper(&args->core.memblock);
    struct gmio_rwargs* core_args = &args->core;
    struct gmio_stl_mesh_creator* mesh_creator = &args->mesh_creator;
    void* mblock_ptr = core_args->memblock.ptr;
    struct gmio_stlb_readwrite_helper rparams = {0};
    struct gmio_stlb_header header;
    uint32_t total_facet_count = 0; /* Facet count, as declared in the stream */
    int error = GMIO_ERROR_OK; /* Helper to store function result error code */
    /* Constants */
    const uint32_t max_facet_count_per_read =
                gmio_size_to_uint32(
                    args->core.memblock.size / GMIO_STLB_TRIANGLE_RAWSIZE);

    /* Check validity of input parameters */
    if (!gmio_stlb_check_params(&error, core_args, byte_order))
        goto label_end;

    /* Initialize rparams */
    if (byte_order != GMIO_ENDIANNESS_HOST)
        rparams.func_fix_endian = gmio_stl_triangle_bswap;

    /* Read header */
    if (gmio_stream_read(&core_args->stream, &header, GMIO_STLB_HEADER_SIZE, 1)
            != 1)
    {
        error = GMIO_STL_ERROR_HEADER_WRONG_SIZE;
        goto label_end;
    }

    /* Read facet count */
    if (gmio_stream_read(&core_args->stream, mblock_ptr, sizeof(uint32_t), 1)
            != 1)
    {
        error = GMIO_STL_ERROR_FACET_COUNT;
        goto label_end;
    }

    memcpy(&total_facet_count, mblock_ptr, sizeof(uint32_t));
    if (byte_order != GMIO_ENDIANNESS_HOST)
        total_facet_count = gmio_uint32_bswap(total_facet_count);

    /* Callback to notify triangle count and header data */
    gmio_stl_mesh_creator_binary_begin_solid(
                mesh_creator, total_facet_count, &header);

    /* Read triangles */
    while (gmio_no_error(error)
           && rparams.i_facet_offset < total_facet_count)
    {
        gmio_rwargs_handle_progress(
                    core_args, rparams.i_facet_offset, total_facet_count);

        rparams.facet_count =
                gmio_size_to_uint32(
                    gmio_stream_read(
                        &core_args->stream,
                        mblock_ptr,
                        GMIO_STLB_TRIANGLE_RAWSIZE,
                        max_facet_count_per_read));
        if (gmio_stream_error(&core_args->stream) != 0)
            error = GMIO_ERROR_STREAM;
        else if (rparams.facet_count > 0)
            error = GMIO_ERROR_OK;
        else
            break; /* Exit if no facet to read */

        if (gmio_no_error(error)) {
            gmio_stlb_read_facets(mesh_creator, mblock_ptr, &rparams);
            rparams.i_facet_offset += rparams.facet_count;
            if (gmio_rwargs_is_stop_requested(core_args))
                error = GMIO_ERROR_TRANSFER_STOPPED;
        }
    } /* end while */

    if (gmio_no_error(error))
        gmio_stl_mesh_creator_end_solid(mesh_creator);

    if (gmio_no_error(error) && rparams.i_facet_offset != total_facet_count)
        error = GMIO_STL_ERROR_FACET_COUNT;

label_end:
    gmio_memblock_helper_release(&mblock_helper);
    return error;
}
