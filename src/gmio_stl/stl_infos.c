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

#include "stl_infos.h"

#include "../gmio_core/error.h"
#include "../gmio_core/internal/helper_memblock.h"
#include "../gmio_core/internal/helper_stream.h"
#include "stl_error.h"
#include "stl_format.h"
#include "internal/stla_infos_get.h"
#include "internal/stlb_infos_get.h"

int gmio_stl_infos_get(
        struct gmio_stl_infos* infos,
        struct gmio_stream stream,
        unsigned flags,
        const struct gmio_stl_infos_get_options* opts)
{
    int error = GMIO_ERROR_OK;
    const struct gmio_streampos begin_streampos = gmio_streampos(&stream, NULL);
    struct gmio_memblock_helper mblock_helper =
            gmio_memblock_helper(opts != NULL ? &opts->stream_memblock : NULL);
    enum gmio_stl_format format =
            opts != NULL ? opts->format_hint : GMIO_STL_FORMAT_UNKNOWN;
    struct gmio_stl_infos_get_options ovrdn_opts = {0};

    if (opts != NULL)
        ovrdn_opts = *opts;
    ovrdn_opts.stream_memblock = mblock_helper.memblock;

    /* Guess format when left unspecified */
    if (format == GMIO_STL_FORMAT_UNKNOWN) {
        format = gmio_stl_get_format(&stream);
        if (format == GMIO_STL_FORMAT_UNKNOWN) {
            error = GMIO_STL_ERROR_UNKNOWN_FORMAT;
            goto label_end;
        }
        ovrdn_opts.format_hint = format;
    }

    if (flags & GMIO_STL_INFO_FLAG_FORMAT)
        infos->format = format;

    /* Dispatch to the sub-function */
    switch (format) {
    case GMIO_STL_FORMAT_ASCII:
        error = gmio_stla_infos_get(infos, stream, flags, &ovrdn_opts);
        break;
    case GMIO_STL_FORMAT_BINARY_LE:
    case GMIO_STL_FORMAT_BINARY_BE:
        error = gmio_stlb_infos_get(infos, stream, flags, &ovrdn_opts);
        break;
    default:
        error = GMIO_STL_ERROR_UNKNOWN_FORMAT;
        break;
    }

label_end:
    gmio_stream_set_pos(&stream, &begin_streampos);
    gmio_memblock_helper_release(&mblock_helper);
    return error;
}

gmio_streamsize_t gmio_stla_infos_get_streamsize(
        struct gmio_stream *stream, struct gmio_memblock *stream_memblock)
{
    struct gmio_stl_infos infos = {0};
    struct gmio_stl_infos_get_options options = {0};
    options.stream_memblock = *stream_memblock;
    options.format_hint = GMIO_STL_FORMAT_ASCII;
    gmio_stl_infos_get(&infos, *stream, GMIO_STL_INFO_FLAG_SIZE, &options);
    return infos.size;
}
