/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
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

/* TODO : documentation */

#ifndef GMIO_INTERNAL_STL_RW_COMMON_H
#define GMIO_INTERNAL_STL_RW_COMMON_H

#include "../../gmio_core/global.h"
#include "../../gmio_core/endian.h"
#include "../../gmio_core/transfer.h"
#include "../stl_mesh.h"
#include "../stl_triangle.h"

struct gmio_stlb_readwrite_helper
{
    uint32_t facet_count;
    uint32_t i_facet_offset;
    void   (*func_fix_endian)(gmio_stl_triangle_t* tri);
};
typedef struct gmio_stlb_readwrite_helper  gmio_stlb_readwrite_helper_t;

gmio_bool_t gmio_check_transfer(int* error, const gmio_transfer_t* trsf);

gmio_bool_t gmio_stl_check_mesh(int* error, const gmio_stl_mesh_t *mesh);

gmio_bool_t gmio_stlb_check_params(
        int* error,
        const gmio_transfer_t* trsf,
        gmio_endianness_t byte_order);

#endif /* GMIO_INTERNAL_STLB_RW_COMMON_H */
