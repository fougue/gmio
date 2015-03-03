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

#ifndef GMIO_LIBSTL_STL_MESH_CREATOR_H
#define GMIO_LIBSTL_STL_MESH_CREATOR_H

#include "stl_global.h"
#include "stl_triangle.h"

/*! Provides an interface for the creation of the underlying(hidden)
 *  user mesh */
struct gmio_stl_mesh_creator
{
    /*! Opaque pointer on the user mesh, passed as first argument to hook
     *  functions */
    void* cookie;

    /* All function pointers are optional (ie can be set to NULL) */

    /*! Pointer on a function that handles declaration of a solid of
     *  name \p solid_name
     *
     *  Optional function useful only with STL ascii (ie. gmio_stla_read())
     */
    void (*ascii_begin_solid_func)(void* cookie, const char* solid_name);

    /*! Pointer on a function that handles declaration of a mesh with
     *  \p tri_count number of triangles
     *
     *  Optional function useful only with STL binary (ie. gmio_stlb_read()).
     *
     *  The argument \p header contains the header data(80 bytes)
     */
    void (*binary_begin_solid_func)(void* cookie,
                                    uint32_t tri_count,
                                    const uint8_t* header);

    /*! Pointer on a function that adds a triangle to the user mesh
     *
     *  The argument \p triangle is the triangle to be added, note that
     *  gmio_stl_triangle_t::attribute_byte_count is meaningless for STL ascii.
     *
     *  The argument \p tri_id is the index of the mesh triangle
     */
    void (*add_triangle_func)(void* cookie,
                              uint32_t tri_id,
                              const gmio_stl_triangle_t* triangle);

    /*! Pointer on a function that finalizes creation of the user mesh
     *
     *  Optional function called at the end of the read process, ie. after all
     *  triangles have been added
     */
    void (*end_solid_func)(void* cookie);
};

typedef struct gmio_stl_mesh_creator gmio_stl_mesh_creator_t;

#endif /* GMIO_LIBSTL_STL_MESH_CREATOR_H */
