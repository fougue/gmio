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

/*! \file stl_mesh_creator.h
 *  Declaration of gmio_stl_mesh_creator
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_MESH_CREATOR_H
#define GMIO_STL_MESH_CREATOR_H

#include "stl_global.h"
#include "stl_triangle.h"
#include "stlb_header.h"

#include <stddef.h>

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
     *
     *  The argument \p stream_size is the total size (in bytes) of the input
     *  stream
     */
    void (*func_ascii_begin_solid)(
            void* cookie, size_t stream_size, const char* solid_name);

    /*! Pointer on a function that handles declaration of a mesh with
     *  \p tri_count number of triangles
     *
     *  Optional function useful only with STL binary (ie. gmio_stlb_read()).
     *
     *  The argument \p header contains the header data(80 bytes)
     */
    void (*func_binary_begin_solid)(
            void* cookie, uint32_t tri_count, const gmio_stlb_header_t* header);

    /*! Pointer on a function that adds a triangle to the user mesh
     *
     *  The argument \p triangle is the triangle to be added, note that
     *  gmio_stl_triangle_t::attribute_byte_count is meaningless for STL ascii.
     *
     *  The argument \p tri_id is the index of the mesh triangle
     */
    void (*func_add_triangle)(
            void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* triangle);

    /*! Pointer on a function that finalizes creation of the user mesh
     *
     *  Optional function called at the end of the read process, ie. after all
     *  triangles have been added
     */
    void (*func_end_solid)(void* cookie);
};

typedef struct gmio_stl_mesh_creator gmio_stl_mesh_creator_t;

#endif /* GMIO_STL_MESH_CREATOR_H */
/*! @} */
