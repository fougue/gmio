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

/*! Finds infos from a STL binary stream */
int gmio_stlb_infos_get(
        struct gmio_stl_infos* infos,
        struct gmio_stream* stream,
        unsigned flags,
        const struct gmio_stl_infos_get_options* opts);

/*! Returns the size(in bytes) of the whole STL binary data given some facet
 *  count */
gmio_streamsize_t gmio_stlb_infos_size(uint32_t facet_count);

#endif /* GMIO_INTERNAL_STLB_INFOS_GET_H */
