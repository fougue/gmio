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

#ifndef GMIO_INTERNAL_STLB_INFOS_GET_H
#define GMIO_INTERNAL_STLB_INFOS_GET_H

#include "../stl_infos.h"
#include "../../gmio_core/endian.h"

int gmio_stlb_infos_get(
        struct gmio_stl_infos_get_args* args,
        enum gmio_endianness byte_order,
        unsigned flags);

#endif /* GMIO_INTERNAL_STLB_INFOS_GET_H */
