/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
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

#include "stlb_infos_probe.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/internal/byte_swap.h"
#include "../../gmio_core/internal/helper_stream.h"

#include <string.h>

static enum gmio_endianness gmio_stl_format_to_endianness(
        enum gmio_stl_format format)
{
    if (format == GMIO_STL_FORMAT_BINARY_BE)
        return GMIO_ENDIANNESS_BIG;
    else if (format == GMIO_STL_FORMAT_BINARY_LE)
        return GMIO_ENDIANNESS_LITTLE;
    return GMIO_ENDIANNESS_UNKNOWN;
}

int gmio_stlb_infos_probe(
        struct gmio_stl_infos* infos,
        struct gmio_stream* stream,
        unsigned flags,
        const struct gmio_stl_infos_probe_options* opts)
{
    if (flags != 0) {
        const enum gmio_endianness byte_order =
                gmio_stl_format_to_endianness(opts->format_hint);
        uint32_t facet_count = 0;
        uint8_t buff[GMIO_STLB_HEADER_SIZE + sizeof(uint32_t)];

        { /* Read header and facet count into buff */
            const size_t read_size =
                    gmio_stream_read_bytes(stream, buff, sizeof(buff));
            if (read_size != sizeof(buff))
                return GMIO_ERROR_STREAM;
        }

        memcpy(&facet_count, buff + GMIO_STLB_HEADER_SIZE, sizeof(uint32_t));
        if (byte_order != GMIO_ENDIANNESS_HOST)
            facet_count = gmio_uint32_bswap(facet_count);

        if (flags & GMIO_STLB_INFO_FLAG_HEADER)
            memcpy(infos->stlb_header.data, buff, GMIO_STLB_HEADER_SIZE);
        if (flags & GMIO_STL_INFO_FLAG_FACET_COUNT)
            infos->facet_count = facet_count;
        if (flags & GMIO_STL_INFO_FLAG_SIZE)
            infos->size = gmio_stlb_infos_size(facet_count);
    }

    return GMIO_ERROR_OK;
}

gmio_streamsize_t gmio_stlb_infos_size(uint32_t facet_count)
{
    return GMIO_STLB_HEADER_SIZE
            + sizeof(uint32_t)
            + facet_count * GMIO_STLB_TRIANGLE_RAWSIZE;
}
