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

#include "stla_write.h"

#include "stl_funptr_typedefs.h"
#include "stl_rw_common.h"
#include "../stl_error.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/internal/helper_stream.h"
#include "../../gmio_core/internal/helper_transfer.h"
#include "../../gmio_core/internal/min_max.h"
#include "../../gmio_core/internal/safe_cast.h"

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

enum { GMIO_STLA_FACET_SIZE = 321 };
enum { GMIO_STLA_FACET_SIZE_P2 = 512 };
enum { GMIO_STLA_SOLID_NAME_MAX_LEN = 512 };

/* Fucntions for raw strings(ie. "const char*") */

GMIO_INLINE char* gmio_write_eol(char* buffer)
{
    *buffer = '\n';
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
        gmio_float_text_format_t format)
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

GMIO_INLINE char* gmio_write_coords(
        char* buffer,
        const char* coords_format,
        const gmio_stl_coords_t* coords)
{
    return buffer + sprintf(buffer,
                            coords_format, coords->x, coords->y, coords->z);
}

GMIO_INLINE gmio_bool_t gmio_transfer_flush_buffer(
        gmio_transfer_t* trsf, size_t n)
{
    const size_t write_count =
            gmio_stream_write(&trsf->stream, trsf->memblock.ptr, sizeof(char), n);
    return write_count == n;
}

int gmio_stla_write(
        gmio_transfer_t* trsf,
        const gmio_stl_mesh_t* mesh,
        const gmio_stl_write_options_t* options)
{
    /* Constants */
    const uint32_t total_facet_count = mesh != NULL ? mesh->triangle_count : 0;
    const uint32_t buffer_facet_count =
            trsf != NULL ?
                gmio_size_to_uint32(trsf->memblock.size / GMIO_STLA_FACET_SIZE_P2)
              : 0;
    const char* opt_solid_name =
            options != NULL ? options->stla_solid_name : NULL;
    const char* solid_name =
            opt_solid_name != NULL ? opt_solid_name : "";
    const gmio_float_text_format_t float32_format =
            options != NULL ?
                options->stla_float32_format :
                GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE;
    const uint8_t float32_prec =
            options != NULL ? options->stla_float32_prec : 9;
    const gmio_bool_t write_triangles_only =
            options != NULL ? options->stl_write_triangles_only : GMIO_FALSE;
    /* Variables */
    uint32_t ifacet = 0;
    void* mblock_ptr = trsf != NULL ? trsf->memblock.ptr : NULL;
    char* buffc = mblock_ptr;
    char coords_format[64];
    int error = GMIO_ERROR_OK;

    /* Check validity of input parameters */
    if (!gmio_check_transfer(&error, trsf))
        return error;
    if (!gmio_stl_check_mesh(&error, mesh))
        return error;
    if (float32_prec == 0 || float32_prec > 9)
        return GMIO_STL_ERROR_INVALID_FLOAT32_PREC;
    if (trsf->memblock.size < GMIO_STLA_FACET_SIZE_P2)
        return GMIO_ERROR_INVALID_MEMBLOCK_SIZE;

    { /* Create XYZ coords format string (for normal and vertex coords) */
        const char float32_specifier =
                gmio_float_text_format_to_specifier(float32_format);
        char* it = coords_format;
        it = gmio_write_stdio_format(it, float32_specifier, float32_prec);
        it = gmio_write_rawstr(it, "  ");
        it = gmio_write_stdio_format(it, float32_specifier, float32_prec);
        it = gmio_write_rawstr(it, "  ");
        it = gmio_write_stdio_format(it, float32_specifier, float32_prec);
        *it = 0; /* Write terminating null byte */
        /* TODO: check the "format" string can contain the given precision */
    }

    /* Write solid declaration */
    if (!write_triangles_only) {
        buffc = gmio_write_rawstr(buffc, "solid ");
        buffc = gmio_write_rawstr_eol(buffc, solid_name);
        if (!gmio_transfer_flush_buffer(trsf, buffc - (char*)mblock_ptr))
            return GMIO_ERROR_STREAM;
    }

    /* Write solid's facets */
    for (ifacet = 0;
         ifacet < total_facet_count && gmio_no_error(error);
         ifacet += buffer_facet_count)
    {
        const gmio_stl_mesh_func_get_triangle_t func_get_triangle =
                mesh->func_get_triangle;
        const void* mesh_cookie = mesh->cookie;
        const uint32_t clamped_facet_count =
                GMIO_MIN(ifacet + buffer_facet_count, total_facet_count);
        gmio_stl_triangle_t tri;
        uint32_t ibuffer_facet;

        gmio_transfer_handle_progress(trsf, ifacet, total_facet_count);

        /* Writing of facets is buffered */
        buffc = mblock_ptr;
        for (ibuffer_facet = ifacet;
             ibuffer_facet < clamped_facet_count;
             ++ibuffer_facet)
        {
            func_get_triangle(mesh_cookie, ibuffer_facet, &tri);
            buffc = gmio_write_rawstr(buffc, "facet normal ");
            buffc = gmio_write_coords(buffc, coords_format, &tri.normal);

            buffc = gmio_write_rawstr(buffc, "\nouter loop");
            buffc = gmio_write_rawstr(buffc, "\n vertex ");
            buffc = gmio_write_coords(buffc, coords_format, &tri.v1);
            buffc = gmio_write_rawstr(buffc, "\n vertex ");
            buffc = gmio_write_coords(buffc, coords_format, &tri.v2);
            buffc = gmio_write_rawstr(buffc, "\n vertex ");
            buffc = gmio_write_coords(buffc, coords_format, &tri.v3);
            buffc = gmio_write_rawstr(buffc, "\nendloop");

            buffc = gmio_write_rawstr(buffc, "\nendfacet\n");
        } /* end for (ibuffer_facet) */

        if (!gmio_transfer_flush_buffer(trsf, buffc - (char*)mblock_ptr))
            error = GMIO_ERROR_STREAM;

        /* Task control */
        if (gmio_no_error(error) && gmio_transfer_is_stop_requested(trsf))
            error = GMIO_ERROR_TRANSFER_STOPPED;
    } /* end for (ifacet) */

    /* Write end of solid */
    if (gmio_no_error(error) && !write_triangles_only) {
        buffc = gmio_write_rawstr(trsf->memblock.ptr, "endsolid ");
        buffc = gmio_write_rawstr_eol(buffc, solid_name);
        if (!gmio_transfer_flush_buffer(trsf, buffc - (char*)mblock_ptr))
            error = GMIO_ERROR_STREAM;
    }

    return error;
}
