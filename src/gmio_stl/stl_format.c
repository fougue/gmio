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

#include "../gmio_core/endian.h"
#include "../gmio_core/internal/byte_codec.h"
#include "../gmio_core/internal/byte_swap.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/min_max.h"
#include "../gmio_core/internal/string_utils.h"

#include <string.h>

enum { GMIO_FIXED_BUFFER_SIZE = 512 };

GMIO_INLINE gmio_streamsize_t gmio_stlb_streamsize(uint32_t facet_count)
{
    return GMIO_STLB_HEADER_SIZE + 4 + facet_count*GMIO_STLB_TRIANGLE_RAWSIZE;
}

enum gmio_stl_format gmio_stl_get_format(struct gmio_stream *stream)
{
    char fixed_buffer[GMIO_FIXED_BUFFER_SIZE] = {0};
    size_t read_size = 0;
    struct gmio_stream_pos stream_start_pos = gmio_stream_pos_null();

    if (stream == NULL)
        return GMIO_STL_FORMAT_UNKNOWN;

    /* Read a chunk of bytes from stream, then try to find format from that
     *
     * First keep stream start position, it will be restored after read
     */
    gmio_stream_get_pos(stream, &stream_start_pos);
    read_size = gmio_stream_read(stream, &fixed_buffer, 1, GMIO_FIXED_BUFFER_SIZE);
    read_size = GMIO_MIN(read_size, GMIO_FIXED_BUFFER_SIZE);
    gmio_stream_set_pos(stream, &stream_start_pos);

    /* Binary STL ? */
    if (read_size >= (GMIO_STLB_HEADER_SIZE + 4)) {
        const gmio_streamsize_t stream_size = gmio_stream_size(stream);

        /* Try with little-endian format */
        uint32_t facet_count =
                gmio_decode_uint32_le((const uint8_t*)fixed_buffer + 80);

        if (gmio_stlb_streamsize(facet_count) == stream_size)
            return GMIO_STL_FORMAT_BINARY_LE;

        /* Try with big-endian format */
        facet_count = gmio_uint32_bswap(facet_count);
        if (gmio_stlb_streamsize(facet_count) == stream_size)
            return GMIO_STL_FORMAT_BINARY_BE;
    }

    /* ASCII STL ? */
    {
        /* Skip spaces at beginning */
        size_t pos = 0;
        while (pos < read_size && gmio_ascii_isspace(fixed_buffer[pos]))
            ++pos;

        /* Next token (if exists) must match "solid\s" */
        if ((pos + 6) < read_size
                && gmio_ascii_istarts_with(fixed_buffer + pos, "solid")
                && gmio_ascii_isspace(fixed_buffer[pos + 5]))
        {
            return GMIO_STL_FORMAT_ASCII;
        }
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
