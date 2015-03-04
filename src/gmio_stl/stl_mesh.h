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

/*! \file stl_mesh.h
 *  Declaration of gmio_stl_mesh
 */

#ifndef GMIO_LIBSTL_STL_MESH_H
#define GMIO_LIBSTL_STL_MESH_H

#include "stl_global.h"
#include "stl_triangle.h"

/*! Provides an interface for accessing the underlying(hidden) user mesh */
struct gmio_stl_mesh
{
    /*! Opaque pointer on the user mesh, passed as first argument to hook
     *  functions */
    const void* cookie;

    /*! Number of triangles in the mesh */
    uint32_t    triangle_count;

    /*! Pointer on a function that stores the mesh triangle of index \p tri_id
     *  into \p triangle */
    void      (*get_triangle_func)(const void* cookie,
                                   uint32_t tri_id,
                                   gmio_stl_triangle_t* triangle);
};

typedef struct gmio_stl_mesh gmio_stl_mesh_t;

#endif /* GMIO_LIBSTL_STL_MESH_H */
