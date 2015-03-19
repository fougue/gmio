/****************************************************************************
**
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
** "http://www.cecill.info".
**
****************************************************************************/

#include "stl_io.h"

#include "internal/stl_rw_common.h"
#include "stl_error.h"

#include "../gmio_core/error.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/helper_transfer.h"
#include "../gmio_core/internal/min_max.h"
#include "../gmio_core/internal/safe_cast.h"

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

static char* gmio_write_string(char* buffer, const char* str)
{
    const char* safe_str = str != NULL ? str : "";
    strcpy(buffer, safe_str);
    return buffer + strlen(safe_str);
}

static char* gmio_write_string_eol(char* buffer, const char* str)
{
    const char* safe_str = str != NULL ? str : "";
    const size_t len = strlen(safe_str);
    strncpy(buffer, safe_str, len);
    buffer[len] = '\n';
    return buffer + len + 1;
}

static char* gmio_write_eol(char* buffer)
{
    *buffer = '\n';
    return buffer + 1;
}

/*static char* gmio_write_space(char* buffer)
{
  *buffer = ' ';
  return buffer + 1;
}*/

static char* gmio_write_nspaces(char* buffer, int n)
{
    const int offset = n;
    while (n > 0)
        *(buffer + (--n)) = ' ';
    return buffer + offset;
}

static char* gmio_write_stdio_format(char* buffer, uint8_t prec)
{
    int prec_len = 0;

    buffer[0] = '%';
    buffer[1] = '.';
    prec_len = sprintf(buffer + 2, "%u", prec);
    buffer[2 + prec_len] = 'E';
    return buffer + 3 + prec_len;
}

static char* gmio_write_coords(
        char* buffer,
        const char* coords_format,
        const gmio_stl_coords_t* coords)
{
    return buffer + sprintf(buffer,
                            coords_format, coords->x, coords->y, coords->z);
}

static gmio_bool_t gmio_transfer_flush_buffer(gmio_transfer_t* trsf, size_t n)
{
    return gmio_stream_write(&trsf->stream, trsf->buffer, sizeof(char), n) == n;
}

int gmio_stla_write(
        const gmio_stl_mesh_t* mesh,
        gmio_transfer_t* trsf,
        const gmio_stla_write_options_t* options)
{
    /* Constants */
    const char* solid_name = options != NULL ? options->solid_name : NULL;
    const uint8_t float32_prec = options != NULL ? options->float32_prec : 9;
    const uint32_t total_facet_count = mesh != NULL ? mesh->triangle_count : 0;
    const uint32_t buffer_facet_count =
            trsf != NULL ?
                gmio_size_to_uint32(trsf->buffer_size / GMIO_STLA_FACET_SIZE_P2)
              : 0;
    /* Variables */
    uint32_t ifacet = 0;
    char* buff_it = trsf != NULL ? trsf->buffer : NULL;
    char coords_format[64];
    int error = GMIO_NO_ERROR;

    /* Check validity of input parameters */
    if (!gmio_check_transfer(&error, trsf))
        return error;
    if (!gmio_stl_check_mesh(&error, mesh))
        return error;
    if (float32_prec == 0 || float32_prec > 9)
        return GMIO_STLA_WRITE_INVALID_REAL32_PREC_ERROR;
    if (trsf->buffer_size < GMIO_STLA_FACET_SIZE_P2)
        return GMIO_INVALID_BUFFER_SIZE_ERROR;

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
        buff_it = gmio_write_string(buff_it, "solid ");
        buff_it = gmio_write_string_eol(buff_it, solid_name);
        if (!gmio_transfer_flush_buffer(trsf, buff_it - (char*)trsf->buffer))
            return GMIO_STREAM_ERROR;
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
        buff_it = trsf->buffer;
        for (ibuffer_facet = ifacet;
             ibuffer_facet < clamped_facet_count;
             ++ibuffer_facet)
        {
            mesh->get_triangle_func(mesh->cookie, ibuffer_facet, &tri);
            buff_it = gmio_write_string(buff_it, "facet normal  ");
            buff_it = gmio_write_coords(buff_it, coords_format, &tri.normal);
            buff_it = gmio_write_eol(buff_it);

            buff_it = gmio_write_string_eol(buff_it, " outer loop");
            buff_it = gmio_write_string(buff_it, "  vertex  ");
            buff_it = gmio_write_coords(buff_it, coords_format, &tri.v1);
            buff_it = gmio_write_eol(buff_it);
            buff_it = gmio_write_string(buff_it, "  vertex  ");
            buff_it = gmio_write_coords(buff_it, coords_format, &tri.v2);
            buff_it = gmio_write_eol(buff_it);
            buff_it = gmio_write_string(buff_it, "  vertex  ");
            buff_it = gmio_write_coords(buff_it, coords_format, &tri.v3);
            buff_it = gmio_write_eol(buff_it);
            buff_it = gmio_write_string_eol(buff_it, " endloop");

            buff_it = gmio_write_string_eol(buff_it, "endfacet");
        } /* end for (ibuffer_facet) */

        if (!gmio_transfer_flush_buffer(trsf, buff_it - (char*)trsf->buffer))
            error = GMIO_STREAM_ERROR;

        /* Task control */
        if (gmio_no_error(error) && gmio_transfer_is_stop_requested(trsf))
            error = GMIO_TRANSFER_STOPPED_ERROR;
    } /* end for (ifacet) */

    /* Write end of solid */
    if (gmio_no_error(error)) {
        buff_it = gmio_write_string(trsf->buffer, "endsolid ");
        buff_it = gmio_write_string_eol(buff_it, solid_name);
        if (!gmio_transfer_flush_buffer(trsf, buff_it - (char*)trsf->buffer))
            error = GMIO_STREAM_ERROR;
    }

    return error;
}
