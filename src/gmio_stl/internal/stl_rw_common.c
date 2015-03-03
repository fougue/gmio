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

#include "stl_rw_common.h"

#include "../../gmio_core/error.h"
#include "../stl_error.h"

gmio_bool_t gmio_check_transfer(int *error, const gmio_transfer_t* trsf)
{
    if (trsf == NULL) {
        *error = GMIO_NULL_TRANSFER_ERROR;
    }
    else {
        if (trsf->buffer == NULL)
            *error = GMIO_NULL_BUFFER_ERROR;
        else if (trsf->buffer_size == 0)
            *error = GMIO_INVALID_BUFFER_SIZE_ERROR;
    }

    return gmio_no_error(*error);
}

gmio_bool_t gmio_stl_check_mesh(int *error, const gmio_stl_mesh_t* mesh)
{
    if (mesh == NULL
            || (mesh->triangle_count > 0 && mesh->get_triangle_func == NULL))
    {
        *error = GMIO_STL_WRITE_NULL_GET_TRIANGLE_FUNC_ERROR;
    }

    return gmio_no_error(*error);
}

gmio_bool_t gmio_stlb_check_params(int *error,
                                   const gmio_transfer_t *trsf,
                                   gmio_endianness_t byte_order)
{
    if (!gmio_check_transfer(error, trsf))
        return GMIO_FALSE;

    if (trsf->buffer_size < GMIO_STLB_MIN_CONTENTS_SIZE)
        *error = GMIO_INVALID_BUFFER_SIZE_ERROR;
    if (byte_order != GMIO_LITTLE_ENDIAN && byte_order != GMIO_BIG_ENDIAN)
        *error = GMIO_STLB_UNSUPPORTED_BYTE_ORDER_ERROR;

    return gmio_no_error(*error);
}
