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
        struct gmio_stream* stream,
        unsigned flags,
        const struct gmio_stl_infos_get_options* opts)
{
    int error = GMIO_ERROR_OK;
    const struct gmio_streampos begin_streampos = gmio_streampos(stream, NULL);
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
        format = gmio_stl_format_probe(stream);
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
    gmio_stream_set_pos(stream, &begin_streampos);
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
    gmio_stl_infos_get(&infos, stream, GMIO_STL_INFO_FLAG_SIZE, &options);
    return infos.size;
}
