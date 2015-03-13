/****************************************************************************
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
****************************************************************************/

/*! \file stl_triangle.h
 *  Declaration of gmio_stl_coords and gmio_stl_triangle
 */

#ifndef GMIO_LIBSTL_TRIANGLE_H
#define GMIO_LIBSTL_TRIANGLE_H

#include "stl_global.h"

/*! Cartesian coordinate entity in 3D space, specifically tailored for
 *  STL needs (single-float) */
struct gmio_stl_coords
{
    gmio_float32_t x;
    gmio_float32_t y;
    gmio_float32_t z;
};

typedef struct gmio_stl_coords gmio_stl_coords_t;

/*! STL mesh triangle defined three geometric vertices and an
 *  orientation(normal) */
struct gmio_stl_triangle
{
    gmio_stl_coords_t normal; /*!< Normal vector */
    gmio_stl_coords_t v1; /*!< Vertex 1 */
    gmio_stl_coords_t v2; /*!< Vertex 2 */
    gmio_stl_coords_t v3; /*!< Vertex 3 */
    uint16_t attribute_byte_count; /*!< Useful only for STL binary format */
};

typedef struct gmio_stl_triangle gmio_stl_triangle_t;

/*! Constants for STL triangles */
enum {
    /*! Compact size of a gmio_stl_coords_t object */
    GMIO_STL_COORDS_RAWSIZE = (3 * sizeof(gmio_float32_t)),

    /*! Compact size of a gmio_stl_triangle_t object for STL ascii format */
    GMIO_STLA_TRIANGLE_RAWSIZE = (4 * GMIO_STL_COORDS_RAWSIZE),

    /*! Compact size of a gmio_stl_triangle_t object for STL binary format */
    GMIO_STLB_TRIANGLE_RAWSIZE = (GMIO_STLA_TRIANGLE_RAWSIZE + sizeof(uint16_t))
};

#endif /* GMIO_LIBSTL_TRIANGLE_H */
