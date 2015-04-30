/****************************************************************************
** GeomIO Library
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

#ifndef GMIO_INTERNAL_STLB_WRITE_H
#define GMIO_INTERNAL_STLB_WRITE_H

#include "../stl_mesh.h"
#include "../../gmio_core/endian.h"
#include "../../gmio_core/transfer.h"

/*! Writes geometry in the STL binary format
 *
 *  \return Error code (see error.h and stl_error.h)
 *  \retval GMIO_INVALID_BUFFER_SIZE_ERROR
 *          if <tt>trsf->buffer.size < GMIO_STLB_MIN_CONTENTS_SIZE</tt>
 */
GMIO_LIBSTL_EXPORT
int gmio_stlb_write(
        gmio_transfer_t* trsf,
        const gmio_stl_mesh_t* mesh,
        /* Options */
        const uint8_t* header_data,
        gmio_endianness_t byte_order);

#endif /* GMIO_INTERNAL_STLB_WRITE_H */
