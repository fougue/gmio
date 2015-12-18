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
#include "internal/stla_infos_get.h"
#include "internal/stlb_infos_get.h"

int gmio_stl_infos_get(
        struct gmio_stl_infos_get_args* args,
        enum gmio_stl_format format,
        unsigned flags)
{
    int error = GMIO_ERROR_OK;
    struct gmio_memblock_helper mblock_helper =
            gmio_memblock_helper(&args->stream_memblock);
    const struct gmio_streampos begin_streampos =
            gmio_streampos(&args->stream, NULL);

    switch (format) {
    case GMIO_STL_FORMAT_ASCII:
        error = gmio_stla_infos_get(args, flags);
        break;
    case GMIO_STL_FORMAT_BINARY_LE:
        error = gmio_stlb_infos_get(args, GMIO_ENDIANNESS_LITTLE, flags);
        break;
    case GMIO_STL_FORMAT_BINARY_BE:
        error = gmio_stlb_infos_get(args, GMIO_ENDIANNESS_BIG, flags);
        break;
    default:
        error = GMIO_STL_ERROR_UNKNOWN_FORMAT;
        break;
    }
    gmio_memblock_helper_release(&mblock_helper);
    gmio_stream_set_pos(&args->stream, &begin_streampos);

    return error;
}
