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
 *  Declaration of gmio_stl_coords and gmio_stl_triangle
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_TRIANGLE_H
#define GMIO_STL_TRIANGLE_H

#include "stl_global.h"

/*! Cartesian coordinate entity in 3D space, specifically tailored for
 *  STL needs (single-float) */
struct gmio_stl_coords
{
    gmio_float32_t x;
    gmio_float32_t y;
    gmio_float32_t z;
};

/*! STL mesh triangle defined three geometric vertices and an
 *  orientation(normal) */
struct gmio_stl_triangle
{
    struct gmio_stl_coords normal; /*!< Normal vector */
    struct gmio_stl_coords v1; /*!< Vertex 1 */
    struct gmio_stl_coords v2; /*!< Vertex 2 */
    struct gmio_stl_coords v3; /*!< Vertex 3 */
    uint16_t attribute_byte_count; /*!< Useful only for STL binary format */
};

/*! Constants for STL triangles */
enum {
    /*! Compact size of a struct gmio_stl_coords object */
    GMIO_STL_COORDS_RAWSIZE = (3 * sizeof(gmio_float32_t)),

    /*! Compact size of a struct gmio_stl_triangle object for STL ascii format */
    GMIO_STLA_TRIANGLE_RAWSIZE = (4 * GMIO_STL_COORDS_RAWSIZE),

    /*! Compact size of a struct gmio_stl_triangle object for STL binary format */
    GMIO_STLB_TRIANGLE_RAWSIZE = (GMIO_STLA_TRIANGLE_RAWSIZE + sizeof(uint16_t))
};

#endif /* GMIO_STL_TRIANGLE_H */
/*! @} */
