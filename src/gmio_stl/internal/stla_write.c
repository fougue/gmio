/****************************************************************************
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
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

GMIO_INLINE char* gmio_write_string(char* buffer, const char* str)
{
    const char* safe_str = str != NULL ? str : "";
    const size_t len = strlen(safe_str);
    strncpy(buffer, safe_str, len);
    return buffer + len;
}

GMIO_INLINE char* gmio_write_eol(char* buffer)
{
    *buffer = '\n';
    return buffer + 1;
}

GMIO_INLINE char* gmio_write_string_eol(char* buffer, const char* str)
{
    buffer = gmio_write_string(buffer, str);
    return gmio_write_eol(buffer);
}

/*static char* gmio_write_space(char* buffer)
{
  *buffer = ' ';
  return buffer + 1;
}*/

GMIO_INLINE char* gmio_write_nspaces(char* buffer, int n)
{
    const int offset = n;
    while (n > 0)
        *(buffer + (--n)) = ' ';
    return buffer + offset;
}

GMIO_INLINE char* gmio_write_stdio_format(char* buffer, uint8_t prec)
{
    int prec_len = 0;

    buffer[0] = '%';
    buffer[1] = '.';
    prec_len = sprintf(buffer + 2, "%u", prec);
    buffer[2 + prec_len] = 'E';
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

static gmio_bool_t gmio_transfer_flush_buffer(gmio_transfer_t* trsf, size_t n)
{
    const size_t write_count =
            gmio_stream_write(&trsf->stream, trsf->buffer.ptr, sizeof(char), n);
    return write_count == n;
}

int gmio_stla_write(
        gmio_transfer_t* trsf,
        const gmio_stl_mesh_t* mesh,
        /* Options */
        const char* solid_name,
        uint8_t float32_prec)
{
    /* Constants */
    const uint32_t total_facet_count = mesh != NULL ? mesh->triangle_count : 0;
    const uint32_t buffer_facet_count =
            trsf != NULL ?
                gmio_size_to_uint32(trsf->buffer.size / GMIO_STLA_FACET_SIZE_P2)
              : 0;
    /* Variables */
    uint32_t ifacet = 0;
    void* buffer_ptr = trsf != NULL ? trsf->buffer.ptr : NULL;
    char* buffc = buffer_ptr;
    char coords_format[64];
    int error = GMIO_ERROR_OK;

    /* Check validity of input parameters */
    if (!gmio_check_transfer(&error, trsf))
        return error;
    if (!gmio_stl_check_mesh(&error, mesh))
        return error;
    if (float32_prec == 0 || float32_prec > 9)
        return GMIO_STL_ERROR_INVALID_REAL32_PREC;
    if (trsf->buffer.size < GMIO_STLA_FACET_SIZE_P2)
        return GMIO_ERROR_INVALID_BUFFER_SIZE;

    { /* Create XYZ coords format string (for normal and vertex coords) */
        char* it = coords_format;
        it = gmio_write_stdio_format(it, float32_prec);
        it = gmio_write_nspaces(it, 2);
        it = gmio_write_stdio_format(it, float32_prec);
        it = gmio_write_nspaces(it, 2);
        it = gmio_write_stdio_format(it, float32_prec);
        *it = 0; /* Write terminating null byte */
        /* TODO: check the "format" string can contain the given precision */
    }

    /* Write solid declaration */
    {
        buffc = gmio_write_string(buffc, "solid ");
        buffc = gmio_write_string_eol(buffc, solid_name);
        if (!gmio_transfer_flush_buffer(trsf, buffc - (char*)buffer_ptr))
            return GMIO_ERROR_STREAM;
    }

    /* Write solid's facets */
    for (ifacet = 0;
         ifacet < total_facet_count && gmio_no_error(error);
         ifacet += buffer_facet_count)
    {
        const uint32_t clamped_facet_count =
                GMIO_MIN(ifacet + buffer_facet_count, total_facet_count);
        gmio_stl_triangle_t tri;
        uint32_t ibuffer_facet;

        gmio_transfer_handle_progress(trsf, ifacet, total_facet_count);

        /* Writing of facets is buffered */
        buffc = buffer_ptr;
        for (ibuffer_facet = ifacet;
             ibuffer_facet < clamped_facet_count;
             ++ibuffer_facet)
        {
            mesh->get_triangle_func(mesh->cookie, ibuffer_facet, &tri);
            buffc = gmio_write_string(buffc, "facet normal ");
            buffc = gmio_write_coords(buffc, coords_format, &tri.normal);
            buffc = gmio_write_eol(buffc);

            buffc = gmio_write_string_eol(buffc, "outer loop");
            buffc = gmio_write_string(buffc, " vertex ");
            buffc = gmio_write_coords(buffc, coords_format, &tri.v1);
            buffc = gmio_write_eol(buffc);
            buffc = gmio_write_string(buffc, " vertex ");
            buffc = gmio_write_coords(buffc, coords_format, &tri.v2);
            buffc = gmio_write_eol(buffc);
            buffc = gmio_write_string(buffc, " vertex ");
            buffc = gmio_write_coords(buffc, coords_format, &tri.v3);
            buffc = gmio_write_eol(buffc);
            buffc = gmio_write_string_eol(buffc, "endloop");

            buffc = gmio_write_string_eol(buffc, "endfacet");
        } /* end for (ibuffer_facet) */

        if (!gmio_transfer_flush_buffer(trsf, buffc - (char*)buffer_ptr))
            error = GMIO_ERROR_STREAM;

        /* Task control */
        if (gmio_no_error(error) && gmio_transfer_is_stop_requested(trsf))
            error = GMIO_ERROR_TRANSFER_STOPPED;
    } /* end for (ifacet) */

    /* Write end of solid */
    if (gmio_no_error(error)) {
        buffc = gmio_write_string(trsf->buffer.ptr, "endsolid ");
        buffc = gmio_write_string_eol(buffc, solid_name);
        if (!gmio_transfer_flush_buffer(trsf, buffc - (char*)buffer_ptr))
            error = GMIO_ERROR_STREAM;
    }

    return error;
}
