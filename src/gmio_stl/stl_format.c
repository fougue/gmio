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

#include "stl_format.h"

#include "stl_triangle.h"
#include "stlb_header.h"
#include "internal/stlb_byte_swap.h"

#include "../gmio_core/endian.h"
#include "../gmio_core/internal/byte_codec.h"
#include "../gmio_core/internal/byte_swap.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/min_max.h"
#include "../gmio_core/internal/numeric_utils.h"
#include "../gmio_core/internal/string_ascii_utils.h"

#include <string.h>

enum { GMIO_FIXED_BUFFER_SIZE = 1024 };

GMIO_INLINE float gmio_sqrlen(const struct gmio_stl_coords* c)
{
    const float cx = c->x;
    const float cy = c->y;
    const float cz = c->z;
    return cx*cx + cy*cy + cz*cz;
}

GMIO_INLINE gmio_streamsize_t gmio_stlb_streamsize(uint32_t facet_count)
{
    return GMIO_STLB_HEADER_SIZE + 4 + facet_count*GMIO_STLB_TRIANGLE_RAWSIZE;
}

/* Does \p str contains <SPC>token ? */
static gmio_bool_t gmio_str_has_token(const char* str, const char* token)
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
            if (gmio_stlb_streamsize(le_facet_count) == stream_size)
                return GMIO_STL_FORMAT_BINARY_LE;
            if (gmio_stlb_streamsize(be_facet_count) == stream_size)
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
            if (gmio_float32_ulp_equals(gmio_sqrlen(&tri.n), 1.f, 100)) {
#ifdef GMIO_HOST_IS_BIG_ENDIAN
                return GMIO_STL_FORMAT_BINARY_BE;
#else
                return GMIO_STL_FORMAT_BINARY_LE;
#endif
            }
            gmio_stl_triangle_bswap(&tri);
            if (gmio_float32_ulp_equals(gmio_sqrlen(&tri.n), 1.f, 100)) {
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

static gmio_bool_t gmio_is_stl_ascii(const char* buff, size_t buff_len)
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
            return GMIO_TRUE;
        }
    }
    return GMIO_FALSE;
}

enum gmio_stl_format gmio_stl_get_format(struct gmio_stream *stream)
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

enum gmio_stl_format gmio_stl_get_format_file(const char* filepath)
{
    enum gmio_stl_format format = GMIO_STL_FORMAT_UNKNOWN;
    FILE* file = fopen(filepath, "rb");

    if (file != NULL) {
        struct gmio_stream stream = gmio_stream_stdio(file);
        format = gmio_stl_get_format(&stream);
        fclose(file);
    }
    return format;
}
