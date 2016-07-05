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

#include "stl_format.h"

#include "stl_triangle.h"
#include "stlb_header.h"
#include "internal/stlb_byte_swap.h"
#include "internal/stlb_infos_get.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/internal/byte_codec.h"
#include "../gmio_core/internal/byte_swap.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/min_max.h"
#include "../gmio_core/internal/numeric_utils.h"
#include "../gmio_core/internal/string_ascii_utils.h"
#include "../gmio_core/internal/vecgeom_utils.h"

#include <string.h>

enum { GMIO_FIXED_BUFFER_SIZE = 1024 };

/* Does \p str contains <SPC>token ? */
static bool gmio_str_has_token(const char* str, const char* token)
{
    const char* substr = gmio_ascii_istrstr(str, token);
    return substr != NULL
            && gmio_ascii_isspace(*(substr - 1));
}

static enum gmio_stl_format gmio_stlb_format(
        struct gmio_stream* stream, const uint8_t* buff, size_t read_size)
{
    if (read_size >= (GMIO_STLB_HEADER_SIZE + 4)) {
        const uint32_t le_facet_count =
                gmio_decode_uint32_le((const uint8_t*)buff + 80);
        const uint32_t be_facet_count =
                gmio_uint32_bswap(le_facet_count);

        /* Assume the stream contains one solid */
        {
            const gmio_streamsize_t stream_size = gmio_stream_size(stream);
            if (gmio_stlb_infos_size(le_facet_count) == stream_size)
                return GMIO_STL_FORMAT_BINARY_LE;
            if (gmio_stlb_infos_size(be_facet_count) == stream_size)
                return GMIO_STL_FORMAT_BINARY_BE;
        }

        /* Tests failed, maybe the stream is made of multiple solids ?
         * Investigate to check if first facet's normal has length ~1.f
         */
        if (le_facet_count > 0) {
            struct gmio_stl_triangle tri;
            memcpy(&tri,
                   buff + GMIO_STLB_HEADER_SIZE + 4,
                   GMIO_STLB_TRIANGLE_RAWSIZE);
            if (gmio_float32_ulp_equals(
                        gmio_vec3f_sqr_length(&tri.n), 1.f, 100))
            {
#ifdef GMIO_HOST_IS_BIG_ENDIAN
                return GMIO_STL_FORMAT_BINARY_BE;
#else
                return GMIO_STL_FORMAT_BINARY_LE;
#endif
            }
            gmio_stl_triangle_bswap(&tri);
            if (gmio_float32_ulp_equals(
                        gmio_vec3f_sqr_length(&tri.n), 1.f, 100))
            {
#ifdef GMIO_HOST_IS_BIG_ENDIAN
                return GMIO_STL_FORMAT_BINARY_LE;
#else
                return GMIO_STL_FORMAT_BINARY_BE;
#endif
            }
        }
    }

    return GMIO_STL_FORMAT_UNKNOWN;
}

static bool gmio_is_stl_ascii(const char* buff, size_t buff_len)
{
    /* Skip spaces at beginning */
    size_t pos = 0;
    while (pos < buff_len && gmio_ascii_isspace(buff[pos]))
        ++pos;

    /* Next token (if exists) must match "solid\s" */
    if ((pos + 6) < buff_len
            && gmio_ascii_istarts_with(buff + pos, "solid")
            && gmio_ascii_isspace(buff[pos + 5]))
    {
        /* Try to find some STL ascii keyword */
        pos += 6;
        if (gmio_str_has_token(buff + pos, "facet")
                || gmio_str_has_token(buff + pos, "endsolid"))
        {
            return true;
        }
    }
    return false;
}

enum gmio_stl_format gmio_stl_format_probe(struct gmio_stream *stream)
{
    char buff[GMIO_FIXED_BUFFER_SIZE] = {0};
    size_t read_size = 0;
    struct gmio_streampos stream_start_pos = {0};

    if (stream == NULL)
        return GMIO_STL_FORMAT_UNKNOWN;

    /* Read a chunk of bytes from stream, then try to find format from that.
     * First keep stream start position, it will be restored after read
     */
    gmio_stream_get_pos(stream, &stream_start_pos);
    read_size = gmio_stream_read(stream, &buff, 1, GMIO_FIXED_BUFFER_SIZE);
    read_size = GMIO_MIN(read_size, GMIO_FIXED_BUFFER_SIZE);
    gmio_stream_set_pos(stream, &stream_start_pos);

    /* Binary STL ? */
    {
        const enum gmio_stl_format format =
                gmio_stlb_format(stream, (const uint8_t*)buff, read_size);
        if (format != GMIO_STL_FORMAT_UNKNOWN)
            return format;
    }

    /* ASCII STL ? */
    if (read_size > 0) {
        /* End buffer with null char for the sake of gmio_str_has_token() */
        const size_t buff_last_i =
                GMIO_MIN(read_size, GMIO_FIXED_BUFFER_SIZE - 1);
        buff[buff_last_i] = 0;
        if (gmio_is_stl_ascii(buff, read_size))
            return GMIO_STL_FORMAT_ASCII;
    }

    /* Fallback case */
    return GMIO_STL_FORMAT_UNKNOWN;
}

enum gmio_stl_format gmio_stl_format_probe_file(const char* filepath)
{
    enum gmio_stl_format format = GMIO_STL_FORMAT_UNKNOWN;
    FILE* file = fopen(filepath, "rb");

    if (file != NULL) {
        struct gmio_stream stream = gmio_stream_stdio(file);
        format = gmio_stl_format_probe(&stream);
        fclose(file);
    }
    return format;
}
