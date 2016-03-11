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
#include "internal/stla_write.h"
#include "internal/stlb_write.h"
#include "../gmio_core/error.h"
#include "../gmio_core/internal/byte_codec.h"
#include "../gmio_core/internal/helper_memblock.h"
#include "../gmio_core/internal/helper_stream.h"

int gmio_stl_read(
        struct gmio_stream* stream,
        struct gmio_stl_mesh_creator* mesh_creator,
        const struct gmio_stl_read_options* options)
{
    const enum gmio_stl_format format = gmio_stl_format_probe(stream);
    switch (format) {
    case GMIO_STL_FORMAT_ASCII:
        return gmio_stla_read(stream, mesh_creator, options);
    case GMIO_STL_FORMAT_BINARY_BE:
        return gmio_stlb_read(
                    stream, mesh_creator, GMIO_ENDIANNESS_BIG, options);
    case GMIO_STL_FORMAT_BINARY_LE:
        return gmio_stlb_read(
                    stream, mesh_creator, GMIO_ENDIANNESS_LITTLE, options);
    case GMIO_STL_FORMAT_UNKNOWN:
        return GMIO_STL_ERROR_UNKNOWN_FORMAT;
    }
    return GMIO_ERROR_UNKNOWN;
}

int gmio_stl_read_file(
        const char* filepath,
        struct gmio_stl_mesh_creator* mesh_creator,
        const struct gmio_stl_read_options* options)
{
    FILE* file = fopen(filepath, "rb");
    if (file != NULL) {
        struct gmio_stream stream = gmio_stream_stdio(file);
        const int error = gmio_stl_read(&stream, mesh_creator, options);
        fclose(file);
        return error;
    }
    return GMIO_ERROR_STDIO;
}

int gmio_stl_write(
        enum gmio_stl_format format,
        struct gmio_stream* stream,
        const struct gmio_stl_mesh* mesh,
        const struct gmio_stl_write_options* options)
{
    switch (format) {
    case GMIO_STL_FORMAT_ASCII:
        return gmio_stla_write(stream, mesh, options);
    case GMIO_STL_FORMAT_BINARY_BE:
        return gmio_stlb_write(GMIO_ENDIANNESS_BIG, stream, mesh, options);
    case GMIO_STL_FORMAT_BINARY_LE:
        return gmio_stlb_write(GMIO_ENDIANNESS_LITTLE, stream, mesh, options);
    case GMIO_STL_FORMAT_UNKNOWN:
        return GMIO_STL_ERROR_UNKNOWN_FORMAT;
    }
    return GMIO_ERROR_UNKNOWN;
}

int gmio_stl_write_file(
        enum gmio_stl_format format,
        const char* filepath,
        const struct gmio_stl_mesh* mesh,
        const struct gmio_stl_write_options* options)
{
    FILE* file = fopen(filepath, "wb");
    if (file != NULL) {
        struct gmio_stream stream = gmio_stream_stdio(file);
        const int error = gmio_stl_write(format, &stream, mesh, options);
        fclose(file);
        return error;
    }
    return GMIO_ERROR_STDIO;
}

static const struct gmio_stlb_header internal_stlb_zero_header = {0};

int gmio_stlb_header_write(
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
