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

/*! \file stl_constants.h
 *  STL constants
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_CONSTANTS_H
#define GMIO_STL_CONSTANTS_H

#include "../gmio_core/global.h"

enum gmio_stl_constants
{
    /*! Compact size of a gmio_stl_coords object */
    GMIO_STL_COORDS_RAWSIZE = (3 * sizeof(gmio_float32_t)),

    /*! Compact size of a gmio_stl_triangle object, STL ascii format */
    GMIO_STLA_TRIANGLE_RAWSIZE = (4 * GMIO_STL_COORDS_RAWSIZE),

    /*! Compact size of a gmio_stl_triangle object, STL binary format */
    GMIO_STLB_TRIANGLE_RAWSIZE = (GMIO_STLA_TRIANGLE_RAWSIZE + sizeof(uint16_t)),

    /*! Size(in bytes) of the header data for STL binary */
    GMIO_STLB_HEADER_SIZE = 80,

    /*! Size(in bytes) of the minimum contents possible with the STL binary
     *  format */
    GMIO_STLB_MIN_CONTENTS_SIZE = 284
};

#endif /* GMIO_STL_CONSTANTS_H */
