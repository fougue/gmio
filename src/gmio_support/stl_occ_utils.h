/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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

/*! \file stl_occ_utils.h
 *  Utility functions for interfacing OpenCascade in gmio
 *
 *  \addtogroup gmio_support
 *  @{
 */

#ifndef GMIO_SUPPORT_STL_OCC_UTILS_H
#define GMIO_SUPPORT_STL_OCC_UTILS_H

#include <gmio_support/support_global.h>

#include <gmio_core/vecgeom.h>
#include <gp_XYZ.hxx>

GMIO_INLINE void gmio_stl_occ_copy_xyz(
        gmio_vec3f* vec, double x, double y, double z)
{
    vec->x = static_cast<float>(x);
    vec->y = static_cast<float>(y);
    vec->z = static_cast<float>(z);
}

GMIO_INLINE void gmio_stl_occ_copy_xyz(
        gmio_vec3f* vec, const gp_XYZ& coords)
{
    gmio_stl_occ_copy_xyz(vec, coords.X(), coords.Y(), coords.Z());
}

#endif /* GMIO_SUPPORT_STL_OCC_UTILS_H */
/*! @} */
