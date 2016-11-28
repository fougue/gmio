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

#include "stla_write.h"

#include "stl_funptr_typedefs.h"
#include "stl_error_check.h"
#include "../stl_error.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/task_iface.h"
#include "../../gmio_core/text_format.h"
#include "../../gmio_core/internal/error_check.h"
#include "../../gmio_core/internal/helper_memblock.h"
#include "../../gmio_core/internal/helper_stream.h"
#include "../../gmio_core/internal/helper_task_iface.h"
#include "../../gmio_core/internal/min_max.h"
#include "../../gmio_core/internal/safe_cast.h"

#if GMIO_FLOAT2STR_LIB == GMIO_FLOAT2STR_LIB_DOUBLE_CONVERSION
#  include "../../gmio_core/internal/google_doubleconversion.h"
#  define gmio_write_coords gmio_write_coords_gdc
#else
#  define gmio_write_coords gmio_write_coords_printf
#endif

#include <stdio.h>
#include <string.h>

/*
 * |     14     ||      17       ||      17       ||     15      |  -> 73
 * facet normal  0.986544556E+00  0.986544556E+00  0.986544556E+00
 * |   11    |
 *  outer loop
 * |   10   ||      17       ||      17       ||     15      |      -> 69
 *   vertex  0.167500112E+02  0.505000112E+02  0.000000000E+00
 *   vertex  0.164599000E+02  0.505000111E+02  0.221480112E+01
 *   vertex  0.166819000E+02  0.483135112E+02  0.221480112E+01
 * |  8   |
 *  endloop
 * |  8   |
 * endfacet
 *
 * Total without EOL = 73 + 11 + 3*69 + 8 + 8 = 307
 * Total with EOL(2 chars) = 307 + 7*2 = 321
 */

enum {
    GMIO_STLA_FACET_SIZE = 321,
    GMIO_STLA_FACET_SIZE_P2 = 512,
    GMIO_STLA_SOLID_NAME_MAX_LEN = 512
};

/* Fucntions for raw strings(ie. "const char*") */

GMIO_INLINE char* gmio_write_eol(char* buffer)
{
    *buffer = '\n';
    return buffer + 1;
}

GMIO_INLINE char* gmio_write_char(char* buffer, char c)
{
    *buffer = c;
    return buffer + 1;
}

GMIO_INLINE char* gmio_write_rawstr(char* buffer, const char* str)
{
    /* pre-condition: str must not be NULL */
#if 0
    const size_t len = strlen(str);
    strncpy(buffer, str, len);
    return buffer + len;
#else
    /* Not that good :
     *     the compiler cannot optimize copy for the target architecture
     */
    while (*str != 0) {
        *buffer = *str;
        ++buffer;
        ++str;
    }
    return buffer;
#endif
}

GMIO_INLINE char* gmio_write_rawstr_eol(char* buffer, const char* str)
{
    buffer = gmio_write_rawstr(buffer, str);
    return gmio_write_eol(buffer);
}

GMIO_INLINE char gmio_float_text_format_to_specifier(
        enum gmio_float_text_format format)
{
    switch (format) {
    case GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE: return 'f';
    case GMIO_FLOAT_TEXT_FORMAT_DECIMAL_UPPERCASE: return 'F';
    case GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_LOWERCASE: return 'e';
    case GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_UPPERCASE: return 'E';
    case GMIO_FLOAT_TEXT_FORMAT_SHORTEST_LOWERCASE: return 'g';
    case GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE: return 'G';
    }
    /* Default, should not be here */
    return GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE;
}

GMIO_INLINE char* gmio_write_stdio_format(
        char* buffer, char format_specifier, uint8_t prec)
{
    int prec_len = 0;

    buffer[0] = '%';
    buffer[1] = '.';
    prec_len = sprintf(buffer + 2, "%u", prec);
    buffer[2 + prec_len] = format_specifier;
    return buffer + 3 + prec_len;
}

struct gmio_vec3f_text_format
{
    enum gmio_float_text_format coord_format;
    uint8_t coord_prec;
    char str_printf_format[32]; /* printf-like format for XYZ coords */
};

GMIO_INLINE char* gmio_write_coords_printf(
        char* buffer,
        const struct gmio_vec3f_text_format* format,
        const struct gmio_vec3f* coords)
{
    return buffer + sprintf(buffer,
                            format->str_printf_format,
                            coords->x, coords->y, coords->z);
}

#if GMIO_FLOAT2STR_LIB == GMIO_FLOAT2STR_LIB_DOUBLE_CONVERSION
GMIO_INLINE char* gmio_write_coords_gdc(
        char* buffer,
        const struct gmio_vec3f_text_format* format,
        const struct gmio_vec3f* coords)
{
    const enum gmio_float_text_format coord_format = format->coord_format;
    const uint8_t coord_prec = format->coord_prec;
    buffer += gmio_float2str_googledoubleconversion(
                coords->x, buffer, 32, coord_format, coord_prec);
    buffer = gmio_write_char(buffer, ' ');
    buffer += gmio_float2str_googledoubleconversion(
                coords->y, buffer, 32, coord_format, coord_prec);
    buffer = gmio_write_char(buffer, ' ');
    buffer += gmio_float2str_googledoubleconversion(
                coords->z, buffer, 32, coord_format, coord_prec);
    return buffer;
}
#endif /* GMIO_FLOAT2STR_LIB_DOUBLE_CONVERSION */

GMIO_INLINE bool gmio_stream_flush_buffer(
        struct gmio_stream* stream, char* buffer, const char* buffer_offset)
{
    const size_t n = buffer_offset - buffer;
    const size_t write_count = gmio_stream_write(stream, buffer, 1, n);
    return write_count == n;
}

int gmio_stla_write(
        struct gmio_stream* stream,
        const struct gmio_stl_mesh* mesh,
        const struct gmio_stl_write_options* opts)
{
    /* Constants */
    static const struct gmio_stl_write_options default_opts = {0};
    const struct gmio_task_iface* task = opts != NULL ? &opts->task_iface : NULL;
    struct gmio_memblock_helper mblock_helper =
            gmio_memblock_helper(opts != NULL ? &opts->stream_memblock : NULL);
    const size_t mblock_size = mblock_helper.memblock.size;
    const uint32_t total_facet_count = mesh != NULL ? mesh->triangle_count : 0;
    const uint32_t buffer_facet_count =
            gmio_size_to_uint32(mblock_size / GMIO_STLA_FACET_SIZE_P2);

    /* Variables */
    struct gmio_memblock* const mblock = &mblock_helper.memblock;
    void* const mblock_ptr = mblock->ptr;
    uint32_t ifacet = 0; /* for-loop counter on facets */
    int error = GMIO_ERROR_OK;
    struct gmio_vec3f_text_format vec_txtformat = {0};

    /* Make options non NULL */
    opts = opts != NULL ? opts : &default_opts;

    /* Initialize helper data for text formatting of vec3f coords */
    vec_txtformat.coord_prec =
            opts->stla_float32_prec != 0 ? opts->stla_float32_prec : 9;
    vec_txtformat.coord_format = opts->stla_float32_format;

    /* Create XYZ coords format string (for normal and vertex coords) */
    {
        const uint8_t f32_prec = vec_txtformat.coord_prec;
        enum gmio_float_text_format f32_format = opts->stla_float32_format;
        const char f32_spec = gmio_float_text_format_to_specifier(f32_format);
        char* buffpos = vec_txtformat.str_printf_format;
        buffpos = gmio_write_stdio_format(buffpos, f32_spec, f32_prec);
        buffpos = gmio_write_char(buffpos, ' ');
        buffpos = gmio_write_stdio_format(buffpos, f32_spec, f32_prec);
        buffpos = gmio_write_char(buffpos, ' ');
        buffpos = gmio_write_stdio_format(buffpos, f32_spec, f32_prec);
        *buffpos = 0;
    }

    /* Check validity of input parameters */
    if (!opts->stla_dont_check_lc_numeric && !gmio_check_lc_numeric(&error))
        goto label_end;
    if (!gmio_check_memblock_size(&error, mblock, GMIO_STLA_FACET_SIZE_P2))
        goto label_end;
    if (!gmio_stl_check_mesh(&error, mesh))
        goto label_end;
    if (!gmio_stla_check_float32_precision(&error, vec_txtformat.coord_prec))
        goto label_end;

    /* Write solid declaration */
    if (!opts->stl_write_triangles_only) {
        char* buffpos = mblock_ptr;
        buffpos = gmio_write_rawstr(buffpos, "solid ");
        if (opts->stla_solid_name != NULL)
            buffpos = gmio_write_rawstr_eol(buffpos, opts->stla_solid_name);
        if (!gmio_stream_flush_buffer(stream, mblock_ptr, buffpos)) {
            error = GMIO_ERROR_STREAM;
            goto label_end;
        }
    }

    /* Write solid's facets */
    for (ifacet = 0;
         ifacet < total_facet_count && gmio_no_error(error);
         ifacet += buffer_facet_count)
    {
        const uint32_t clamped_facet_count =
                GMIO_MIN(ifacet + buffer_facet_count, total_facet_count);
        struct gmio_stl_triangle tri;
        uint32_t ibuffer_facet;
        char* buffpos = mblock_ptr;

        gmio_task_iface_handle_progress(task, ifacet, total_facet_count);

        /* Writing of facets is buffered */
        for (ibuffer_facet = ifacet;
             ibuffer_facet < clamped_facet_count;
             ++ibuffer_facet)
        {
            mesh->func_get_triangle(mesh->cookie, ibuffer_facet, &tri);

            buffpos = gmio_write_rawstr(buffpos, "facet normal ");
            buffpos = gmio_write_coords(buffpos, &vec_txtformat, &tri.n);

            buffpos = gmio_write_rawstr(buffpos, "\nouter loop");
            buffpos = gmio_write_rawstr(buffpos, "\n vertex ");
            buffpos = gmio_write_coords(buffpos, &vec_txtformat, &tri.v1);
            buffpos = gmio_write_rawstr(buffpos, "\n vertex ");
            buffpos = gmio_write_coords(buffpos, &vec_txtformat, &tri.v2);
            buffpos = gmio_write_rawstr(buffpos, "\n vertex ");
            buffpos = gmio_write_coords(buffpos, &vec_txtformat, &tri.v3);
            buffpos = gmio_write_rawstr(buffpos, "\nendloop");

            buffpos = gmio_write_rawstr(buffpos, "\nendfacet\n");
        } /* end for (ibuffer_facet) */

        if (!gmio_stream_flush_buffer(stream, mblock_ptr, buffpos))
            error = GMIO_ERROR_STREAM;

        /* Task control */
        if (gmio_no_error(error) && gmio_task_iface_is_stop_requested(task))
            error = GMIO_ERROR_TRANSFER_STOPPED;
    } /* end for (ifacet) */

    /* Write end of solid */
    if (gmio_no_error(error) && !opts->stl_write_triangles_only) {
        char* buffpos = mblock_ptr;
        buffpos = gmio_write_rawstr(buffpos, "endsolid ");
        if (opts->stla_solid_name != NULL)
            buffpos = gmio_write_rawstr_eol(buffpos, opts->stla_solid_name);
        if (!gmio_stream_flush_buffer(stream, mblock_ptr, buffpos))
            error = GMIO_ERROR_STREAM;
    }

label_end:
    gmio_memblock_helper_release(&mblock_helper);
    return error;
}
