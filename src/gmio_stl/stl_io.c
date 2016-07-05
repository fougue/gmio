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
