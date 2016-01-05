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

#include "stl_rw_common.h"

#include "../../gmio_core/error.h"
#include "../../gmio_core/rwargs.h"
#include "../stl_error.h"
#include "../stl_io.h"

gmio_bool_t gmio_check_rwargs(int *error, const struct gmio_rwargs* args)
{
    if (args == NULL) {
        *error = GMIO_ERROR_NULL_RWARGS;
    }
    else {
        if (args->stream_memblock.ptr == NULL)
            *error = GMIO_ERROR_NULL_MEMBLOCK;
        else if (args->stream_memblock.size == 0)
            *error = GMIO_ERROR_INVALID_MEMBLOCK_SIZE;
    }

    return gmio_no_error(*error);
}

gmio_bool_t gmio_check_memblock(int *error, const struct gmio_memblock* mblock)
{
    if (mblock->ptr == NULL)
        *error = GMIO_ERROR_NULL_MEMBLOCK;
    else if (mblock->size == 0)
        *error = GMIO_ERROR_INVALID_MEMBLOCK_SIZE;
    return gmio_no_error(*error);
}

gmio_bool_t gmio_stl_check_mesh(int *error, const struct gmio_stl_mesh* mesh)
{
    if (mesh == NULL
            || (mesh->triangle_count > 0 && mesh->func_get_triangle == NULL))
    {
        *error = GMIO_STL_ERROR_NULL_FUNC_GET_TRIANGLE;
    }

    return gmio_no_error(*error);
}

gmio_bool_t gmio_stlb_check_params(
        int *error,
        const struct gmio_rwargs* args,
        enum gmio_endianness byte_order)
{
    if (!gmio_check_rwargs(error, args))
        return GMIO_FALSE;

    if (args->stream_memblock.size < GMIO_STLB_MIN_CONTENTS_SIZE)
        *error = GMIO_ERROR_INVALID_MEMBLOCK_SIZE;
    if (byte_order != GMIO_ENDIANNESS_LITTLE
            && byte_order != GMIO_ENDIANNESS_BIG)
    {
        *error = GMIO_STL_ERROR_UNSUPPORTED_BYTE_ORDER;
    }

    return gmio_no_error(*error);
}
