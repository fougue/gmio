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

/* TODO : documentation */

#ifndef GMIO_INTERNAL_STL_RW_COMMON_H
#define GMIO_INTERNAL_STL_RW_COMMON_H

#include "stl_funptr_typedefs.h"

#include "../../gmio_core/global.h"
#include "../../gmio_core/endian.h"

struct gmio_memblock;
struct gmio_rwargs;
struct gmio_stl_mesh;

bool gmio_check_rwargs(int* error, const struct gmio_rwargs* args);
bool gmio_check_memblock(int* error, const struct gmio_memblock* mblock);

bool gmio_stl_check_mesh(int* error, const struct gmio_stl_mesh* mesh);

bool gmio_stlb_check_params(
        int* error,
        const struct gmio_rwargs* args,
        enum gmio_endianness byte_order);

#endif /* GMIO_INTERNAL_STLB_RW_COMMON_H */
