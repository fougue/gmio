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

/*! \file vecgeom.h
 *  Cartesian vectors
 *
 *  \addtogroup gmio_core
 *  @{
 */

#ifndef GMIO_CORE_VECGEOM_H
#define GMIO_CORE_VECGEOM_H

/*! Vector of three float coords */
struct gmio_vec3f
{
    float x;
    float y;
    float z;
};

/*! Vector of three double coords */
struct gmio_vec3d
{
    double x;
    double y;
    double z;
};

#endif /* GMIO_CORE_VECGEOM_H */
/*! @} */
