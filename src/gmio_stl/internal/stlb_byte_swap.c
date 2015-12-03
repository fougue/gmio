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

#include "stlb_byte_swap.h"

#include "../../gmio_core/internal/byte_swap.h"

void gmio_stl_triangle_bswap(struct gmio_stl_triangle* triangle)
{
    int i;
    uint32_t* uintcoord_ptr = (uint32_t*)&(triangle->normal.x);
    const uint16_t attr_byte_count = triangle->attribute_byte_count;

    for (i = 0; i < 12; ++i)
        *(uintcoord_ptr + i) = gmio_uint32_bswap(*(uintcoord_ptr + i));
    if (attr_byte_count != 0)
        triangle->attribute_byte_count = gmio_uint16_bswap(attr_byte_count);
}
