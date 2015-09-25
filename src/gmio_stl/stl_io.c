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

#include "stl_io.h"

#include "stl_error.h"
#include "stl_format.h"
#include "internal/stla_write.h"
#include "internal/stlb_write.h"
#include "../gmio_core/error.h"
#include "../gmio_core/stream.h"
#include "../gmio_core/transfer.h"
#include "../gmio_core/internal/byte_codec.h"
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
        gmio_transfer_t trsf = {0};
        trsf.stream = gmio_stream_stdio(file);
        trsf.memblock = gmio_memblock_default();
        if (task_iface != NULL)
            trsf.task_iface = *task_iface;

        error = gmio_stl_read(&trsf, creator);
        fclose(file);
        gmio_memblock_deallocate(&trsf.memblock);
    }
    else {
        error = GMIO_ERROR_STDIO;
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
        gmio_transfer_t trsf = {0};
        trsf.stream = gmio_stream_stdio(file);
        trsf.memblock = gmio_memblock_default();
        if (task_iface != NULL)
            trsf.task_iface = *task_iface;

        error = gmio_stl_write(format, &trsf, mesh, options);
        fclose(file);
        gmio_memblock_deallocate(&trsf.memblock);
    }
    else {
        error = GMIO_ERROR_STDIO;
    }

    return error;
}

int gmio_stl_write(
        gmio_stl_format_t format,
        gmio_transfer_t *trsf,
        const gmio_stl_mesh_t *mesh,
        const gmio_stl_write_options_t *options)
{
    int error = GMIO_ERROR_OK;

    if (trsf != NULL) {
        switch (format) {
        case GMIO_STL_FORMAT_ASCII: {
            error = gmio_stla_write(trsf, mesh, options);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_BE: {
            error = gmio_stlb_write(trsf, mesh, options, GMIO_ENDIANNESS_BIG);
            break;
        }
        case GMIO_STL_FORMAT_BINARY_LE: {
            error = gmio_stlb_write(trsf, mesh, options, GMIO_ENDIANNESS_LITTLE);
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

static const gmio_stlb_header_t internal_stlb_zero_header = {0};

int gmio_stlb_write_header(
        gmio_stream_t *stream,
        gmio_endianness_t byte_order,
        const gmio_stlb_header_t *header,
        uint32_t facet_count)
{
    uint8_t facet_count_bytes[sizeof(uint32_t)];
    const gmio_stlb_header_t* non_null_header =
            header != NULL ? header : &internal_stlb_zero_header;

    /* Write 80-byte header */
    if (gmio_stream_write(stream, non_null_header, GMIO_STLB_HEADER_SIZE, 1)
            != 1)
    {
        return GMIO_ERROR_STREAM;
    }

    /* Write facet count */
    if (byte_order == GMIO_ENDIANNESS_LITTLE)
        gmio_encode_uint32_le(facet_count, &facet_count_bytes[0]);
    else
        gmio_encode_uint32_be(facet_count, &facet_count_bytes[0]);
    if (gmio_stream_write(stream, &facet_count_bytes[0], sizeof(uint32_t), 1)
            != 1)
    {
        return GMIO_ERROR_STREAM;
    }

    return GMIO_ERROR_OK;
}
