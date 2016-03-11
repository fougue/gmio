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

/*! \file stl_triangle.h
 *  Declaration of gmio_stl_triangle
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_TRIANGLE_H
#define GMIO_STL_TRIANGLE_H

#include "stl_global.h"
#include "../gmio_core/vecgeom.h"

/*! STL mesh triangle */
struct gmio_stl_triangle
{
    struct gmio_vec3_f32 n; /*!< Normal vector */
    struct gmio_vec3_f32 v1; /*!< Vertex 1 */
    struct gmio_vec3_f32 v2; /*!< Vertex 2 */
    struct gmio_vec3_f32 v3; /*!< Vertex 3 */
    uint16_t attribute_byte_count; /*!< Useful only for STL binary format */
};

GMIO_C_LINKAGE_BEGIN

/*! Computes the normal vector of triangle \p tri */
GMIO_API void gmio_stl_triangle_compute_normal(struct gmio_stl_triangle* tri);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_TRIANGLE_H */
/*! @} */
