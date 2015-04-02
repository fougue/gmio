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

#include "stl_io.h"

#include "stl_error.h"
#include "stl_format.h"
#include "internal/stla_write.h"
#include "internal/stlb_write.h"
#include "../gmio_core/error.h"
#include "../gmio_core/stream.h"
#include "../gmio_core/transfer.h"
#include "../gmio_core/internal/helper_stream.h"

int gmio_stl_read_file(
        const char* filepath,
        gmio_stl_mesh_creator_t* creator,
        gmio_task_iface_t* task_iface)
{
    int error = GMIO_ERROR_OK;
    FILE* file = NULL;

    file = fopen(filepath, "rb");
    if (file != NULL) {
        gmio_transfer_t trsf = { 0 };
        trsf.stream = gmio_stream_stdio(file);
        trsf.buffer = gmio_buffer_default();
        if (task_iface != NULL)
            trsf.task_iface = *task_iface;

        error = gmio_stl_read(&trsf, creator);
        fclose(file);
        gmio_buffer_deallocate(&trsf.buffer);
    }
    else {
        error = GMIO_ERROR_UNKNOWN;
    }

    return error;
}

int gmio_stl_read(gmio_transfer_t *trsf, gmio_stl_mesh_creator_t *creator)
{
    int error = GMIO_ERROR_OK;

    if (trsf != NULL) {
        const gmio_stl_format_t stl_format = gmio_stl_get_format(&trsf->stream);

        switch (stl_format) {
        case GMIO_STL_FORMAT_ASCII: {
            error = gmio_stla_read(trsf, creator);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_BE: {
            error = gmio_stlb_read(trsf, creator, GMIO_ENDIANNESS_BIG);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_LE: {
            error = gmio_stlb_read(trsf, creator, GMIO_ENDIANNESS_LITTLE);
            break;
        }
        case GMIO_STL_FORMAT_UNKNOWN: {
            error = GMIO_STL_ERROR_UNKNOWN_FORMAT;
        }
        } /* end switch() */
    }
    else {
        error = GMIO_ERROR_NULL_TRANSFER;
    }

    return error;
}

int gmio_stl_write_file(
        gmio_stl_format_t format,
        const char *filepath,
        const gmio_stl_mesh_t *mesh,
        gmio_task_iface_t *task_iface,
        const gmio_stl_write_options_t *options)
{
    int error = GMIO_ERROR_OK;
    FILE* file = NULL;

    file = fopen(filepath, "wb");
    if (file != NULL) {
        gmio_transfer_t trsf = { 0 };
        trsf.stream = gmio_stream_stdio(file);
        trsf.buffer = gmio_buffer_default();
        if (task_iface != NULL)
            trsf.task_iface = *task_iface;

        error = gmio_stl_write(format, &trsf, mesh, options);
        fclose(file);
        gmio_buffer_deallocate(&trsf.buffer);
    }
    else {
        error = GMIO_ERROR_UNKNOWN;
    }

    return error;
}

int gmio_stl_write(
        gmio_stl_format_t format,
        gmio_transfer_t *trsf,
        const gmio_stl_mesh_t *mesh,
        const gmio_stl_write_options_t *options)
{
    const uint8_t* header_data =
            options != NULL ? options->stlb_header_data : NULL;
    int error = GMIO_ERROR_OK;

    if (trsf != NULL) {
        switch (format) {
        case GMIO_STL_FORMAT_ASCII: {
            const char* solid_name =
                    options != NULL ? options->stla_solid_name : NULL;
            const uint8_t float32_prec =
                    options != NULL ? options->stla_float32_prec : 9;
            error = gmio_stla_write(trsf, mesh, solid_name, float32_prec);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_BE: {
            error = gmio_stlb_write(
                        trsf, mesh, header_data, GMIO_ENDIANNESS_BIG);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_LE: {
            error = gmio_stlb_write(
                        trsf, mesh, header_data, GMIO_ENDIANNESS_LITTLE);
            break;
        }
        case GMIO_STL_FORMAT_UNKNOWN: {
            error = GMIO_STL_ERROR_UNKNOWN_FORMAT;
        }
        } /* end switch() */
    }
    else {
        error = GMIO_ERROR_NULL_TRANSFER;
    }

    return error;
}
