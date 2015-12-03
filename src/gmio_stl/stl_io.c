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
#include "stl_format.h"
#include "stlb_header.h"
#include "internal/stla_write.h"
#include "internal/stlb_write.h"
#include "../gmio_core/error.h"
#include "../gmio_core/rwargs.h"
#include "../gmio_core/stream.h"
#include "../gmio_core/internal/byte_codec.h"
#include "../gmio_core/internal/helper_stream.h"

int gmio_stl_read_file(
        const char* filepath,
        struct gmio_rwargs* args,
        struct gmio_stl_mesh_creator* creator)
{
    int error = GMIO_ERROR_OK;
    FILE* file = NULL;

    file = fopen(filepath, "rb");
    if (file != NULL) {
        struct gmio_rwargs local_args =
                args != NULL ? *args : gmio_rwargs_null();
        const gmio_bool_t memblock_allocated =
                local_args.memblock.ptr == NULL;

        if (memblock_allocated)
            local_args.memblock = gmio_memblock_default();
        local_args.stream = gmio_stream_stdio(file);
        error = gmio_stl_read(&local_args, creator);
        fclose(file);
        if (memblock_allocated)
            gmio_memblock_deallocate(&local_args.memblock);
    }
    else {
        error = GMIO_ERROR_STDIO;
    }

    return error;
}

int gmio_stl_read(
        struct gmio_rwargs* args,
        struct gmio_stl_mesh_creator* creator)
{
    int error = GMIO_ERROR_OK;

    if (args != NULL) {
        const enum gmio_stl_format stl_format =
                gmio_stl_get_format(&args->stream);

        switch (stl_format) {
        case GMIO_STL_FORMAT_ASCII: {
            error = gmio_stla_read(args, creator);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_BE: {
            error = gmio_stlb_read(args, creator, GMIO_ENDIANNESS_BIG);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_LE: {
            error = gmio_stlb_read(args, creator, GMIO_ENDIANNESS_LITTLE);
            break;
        }
        case GMIO_STL_FORMAT_UNKNOWN: {
            error = GMIO_STL_ERROR_UNKNOWN_FORMAT;
        }
        } /* end switch() */
    }
    else {
        error = GMIO_ERROR_NULL_RWARGS;
    }

    return error;
}

int gmio_stl_write_file(
        const char* filepath,
        struct gmio_rwargs* args,
        const struct gmio_stl_mesh* mesh,
        enum gmio_stl_format format,
        const struct gmio_stl_write_options* options)
{
    int error = GMIO_ERROR_OK;
    FILE* file = NULL;

    file = fopen(filepath, "wb");
    if (file != NULL) {
        struct gmio_rwargs local_args =
                args != NULL ? *args : gmio_rwargs_null();
        const gmio_bool_t memblock_allocated =
                local_args.memblock.ptr == NULL;

        if (memblock_allocated)
            local_args.memblock = gmio_memblock_default();
        local_args.stream = gmio_stream_stdio(file);
        error = gmio_stl_write(&local_args, mesh, format, options);
        fclose(file);
        if (memblock_allocated)
            gmio_memblock_deallocate(&local_args.memblock);
    }
    else {
        error = GMIO_ERROR_STDIO;
    }

    return error;
}

int gmio_stl_write(
        struct gmio_rwargs* args,
        const struct gmio_stl_mesh* mesh,
        enum gmio_stl_format format,
        const struct gmio_stl_write_options* options)
{
    int error = GMIO_ERROR_OK;

    if (args != NULL) {
        const gmio_bool_t memblock_allocated = args->memblock.ptr == NULL;
        if (memblock_allocated)
            args->memblock = gmio_memblock_default();

        switch (format) {
        case GMIO_STL_FORMAT_ASCII: {
            error = gmio_stla_write(args, mesh, options);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_BE: {
            error = gmio_stlb_write(args, mesh, options, GMIO_ENDIANNESS_BIG);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_LE: {
            error = gmio_stlb_write(args, mesh, options, GMIO_ENDIANNESS_LITTLE);
            break;
        }
        case GMIO_STL_FORMAT_UNKNOWN: {
            error = GMIO_STL_ERROR_UNKNOWN_FORMAT;
        }
        } /* end switch() */

        if (memblock_allocated)
            gmio_memblock_deallocate(&args->memblock);
    }
    else {
        error = GMIO_ERROR_NULL_RWARGS;
    }

    return error;
}

static const struct gmio_stlb_header internal_stlb_zero_header = {0};

int gmio_stlb_write_header(
        struct gmio_stream *stream,
        enum gmio_endianness byte_order,
        const struct gmio_stlb_header *header,
        uint32_t facet_count)
{
    uint8_t facet_count_bytes[sizeof(uint32_t)];
    const struct gmio_stlb_header* non_null_header =
            header != NULL ? header : &internal_stlb_zero_header;

    /* Write 80-byte header */
    if (gmio_stream_write(stream, non_null_header, GMIO_STLB_HEADER_SIZE, 1)
            != 1)
    {
        return GMIO_ERROR_STREAM;
    }

    /* Write facet count */
    if (byte_order == GMIO_ENDIANNESS_LITTLE)
        gmio_encode_uint32_le(facet_count, facet_count_bytes);
    else
        gmio_encode_uint32_be(facet_count, facet_count_bytes);
    if (gmio_stream_write(stream, facet_count_bytes, sizeof(uint32_t), 1) != 1)
        return GMIO_ERROR_STREAM;

    return GMIO_ERROR_OK;
}
