/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/helper_task_iface.h"
#include "../gmio_core/internal/min_max.h"
#include "../gmio_core/internal/safe_cast.h"

#include <string.h>

GMIO_INLINE void decode_facet(
        const uint8_t* buffer, struct gmio_stl_triangle* triangle)
{
    /* *triangle = *((struct gmio_stl_triangle*)(buffer)); */
    memcpy(triangle, buffer, GMIO_STLB_TRIANGLE_RAWSIZE);
}

typedef void (*func_gmio_stlb_decode_facets_t)(
        struct gmio_stl_mesh_creator*,
        const uint8_t*,  /* buffer */
        const uint32_t,  /* facet_count */
        const uint32_t); /* i_facet_offset */

static void gmio_stlb_decode_facets(
        struct gmio_stl_mesh_creator* creator,
        const uint8_t* buffer,
        const uint32_t facet_count,
        const uint32_t i_facet_offset)
{
    const gmio_stl_mesh_creator_func_add_triangle_t func_add_triangle =
            creator->func_add_triangle;
    void* cookie = creator->cookie;

    if (func_add_triangle != NULL) {
        struct gmio_stl_triangle triangle;
        uint32_t i_facet;
        for (i_facet = 0; i_facet < facet_count; ++i_facet) {
            decode_facet(buffer, &triangle);
            buffer += GMIO_STLB_TRIANGLE_RAWSIZE;
            func_add_triangle(cookie, i_facet_offset + i_facet, &triangle);
        }
    }
}

static void gmio_stlb_decode_facets_byteswap(
        struct gmio_stl_mesh_creator* creator,
        const uint8_t* buffer,
        const uint32_t facet_count,
        const uint32_t i_facet_offset)
{
    const gmio_stl_mesh_creator_func_add_triangle_t func_add_triangle =
            creator->func_add_triangle;
    void* cookie = creator->cookie;

    if (func_add_triangle != NULL) {
        struct gmio_stl_triangle triangle;
        uint32_t i_facet;
        for (i_facet = 0; i_facet < facet_count; ++i_facet) {
            decode_facet(buffer, &triangle);
            buffer += GMIO_STLB_TRIANGLE_RAWSIZE;
            gmio_stl_triangle_bswap(&triangle);
            func_add_triangle(cookie, i_facet_offset + i_facet, &triangle);
        }
    }
}

int gmio_stlb_read(
        struct gmio_stream* stream,
        struct gmio_stl_mesh_creator* mesh_creator,
        enum gmio_endianness byte_order,
        const struct gmio_stl_read_options* opts)
{
    /* Variables */
    struct gmio_memblock_helper mblock_helper =
            gmio_memblock_helper(opts != NULL ? &opts->stream_memblock : NULL);
    struct gmio_memblock* mblock = &mblock_helper.memblock;
    const struct gmio_task_iface* task = opts != NULL ? &opts->task_iface : NULL;
    struct gmio_stlb_header header;
    uint32_t i_facet = 0; /* Facet counter */
    uint32_t total_facet_count = 0; /* Facet count, as declared in the stream */
    int error = GMIO_ERROR_OK; /* Function result(error code) */
    /* Constants */
    const func_gmio_stlb_decode_facets_t func_decode_facets =
            byte_order != GMIO_ENDIANNESS_HOST ?
                gmio_stlb_decode_facets_byteswap :
                gmio_stlb_decode_facets;
    const uint32_t max_facet_count_per_read =
            gmio_size_to_uint32(mblock->size / GMIO_STLB_TRIANGLE_RAWSIZE);

    /* Check validity of input parameters */
    if (!gmio_check_memblock_size(&error, mblock, GMIO_STLB_MIN_CONTENTS_SIZE))
        goto label_end;
    if (!gmio_stlb_check_byteorder(&error, byte_order))
        goto label_end;

    /* Read header */
    if (gmio_stream_read(stream, &header, GMIO_STLB_HEADER_SIZE, 1) != 1) {
        error = GMIO_STL_ERROR_HEADER_WRONG_SIZE;
        goto label_end;
    }

    /* Read facet count */
    if (gmio_stream_read(stream, mblock->ptr, sizeof(uint32_t), 1) != 1) {
        error = GMIO_STL_ERROR_FACET_COUNT;
        goto label_end;
    }
    memcpy(&total_facet_count, mblock->ptr, sizeof(uint32_t));
    if (byte_order != GMIO_ENDIANNESS_HOST)
        total_facet_count = gmio_uint32_bswap(total_facet_count);

    /* Callback to notify triangle count and header data */
    {
        struct gmio_stl_mesh_creator_infos infos = {0};
        infos.format =
                byte_order == GMIO_ENDIANNESS_LITTLE ?
                    GMIO_STL_FORMAT_BINARY_LE :
                    GMIO_STL_FORMAT_BINARY_BE;
        infos.stlb_header = &header;
        infos.stlb_triangle_count = total_facet_count;
        gmio_stl_mesh_creator_begin_solid(mesh_creator, &infos);
    }

    /* Read triangles */
    gmio_task_iface_handle_progress(task, 0, total_facet_count);
    while (gmio_no_error(error) && i_facet < total_facet_count) {
        const uint32_t facet_count_to_read =
                GMIO_MIN(max_facet_count_per_read,
                         total_facet_count - i_facet);
        const uint32_t read_facet_count =
                gmio_size_to_uint32(
                    gmio_stream_read(
                        stream,
                        mblock->ptr,
                        GMIO_STLB_TRIANGLE_RAWSIZE,
                        facet_count_to_read));

        if (gmio_stream_error(stream) != 0)
            error = GMIO_ERROR_STREAM;
        else if (read_facet_count > 0)
            error = GMIO_ERROR_OK;
        else
            break; /* Exit if no facet to read */

        if (gmio_no_error(error)) {
            func_decode_facets(
                        mesh_creator, mblock->ptr, read_facet_count, i_facet);
            i_facet += read_facet_count;
            if (gmio_task_iface_is_stop_requested(task))
                error = GMIO_ERROR_TRANSFER_STOPPED;
        }
        gmio_task_iface_handle_progress(task, i_facet, total_facet_count);
    } /* end while */

    if (gmio_no_error(error))
        gmio_stl_mesh_creator_end_solid(mesh_creator);

    if (gmio_no_error(error) && i_facet != total_facet_count)
        error = GMIO_STL_ERROR_FACET_COUNT;

label_end:
    gmio_memblock_helper_release(&mblock_helper);
    return error;
}
