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

#ifndef GMIO_INTERNAL_STLB_BYTE_SWAP_H
#define GMIO_INTERNAL_STLB_BYTE_SWAP_H

#include "../../gmio_core/global.h"
#include "../stl_triangle.h"

/*! Specific byte-swap of the memory occupied by a gmio_stl_triangle object
 *
 *  Each XYZ coord (float32) is individually reversed (byte-swap) as well as
 *  the "attribute byte count" member.
 */
void gmio_stl_triangle_bswap(gmio_stl_triangle_t* triangle);

#endif /* GMIO_INTERNAL_STLB_BYTE_SWAP_H */
