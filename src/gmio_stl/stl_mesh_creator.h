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
#include "stl_format.h"
#include "stl_triangle.h"
#include "stlb_header.h"
#include "../gmio_core/stream.h"

#include <stddef.h>

/*! Informations about the STL stream, used in
 *  gmio_stl_mesh_creator::func_begin_solid() */
struct gmio_stl_mesh_creator_infos
{
    /*! Format of the input STL mesh */
    enum gmio_stl_format format;

    /*! Null terminated C-string holding the STL mesh(solid) name
     *
     *  Available only if STL ASCII format, \c NULL otherwise
     */
    const char* stla_solid_name;

    /*! Total size (in bytes) of the input stream
     *
     *  This is the result of gmio_stl_read_options::func_stla_get_streamsize()
     *
     *  Useful to roughly estimate the facet count in the input mesh.
     *  Available only if STL ASCII format, \c 0 otherwise
     */
    gmio_streamsize_t stla_stream_size;

    /*! Contains the header data(80 bytes)
     *
     *  Available only if binary STL, \c NULL otherwise
     */
    const struct gmio_stlb_header* stlb_header;

    /*! Count of mesh facets(triangles)
     *
     *  Available only if binary STL, \c 0 otherwise
     */
    uint32_t stlb_triangle_count;
};

/*! Provides an interface for the creation of the underlying(hidden)
 * user mesh */
struct gmio_stl_mesh_creator
{
    /*! Opaque pointer on the user mesh, passed as first argument to hook
     *  functions */
    void* cookie;

    /* All function pointers are optional (ie can be set to NULL) */

    /*! Optional pointer on a function that handles declaration of a solid */
    void (*func_begin_solid)(
            void* cookie, const struct gmio_stl_mesh_creator_infos* infos);

    /*! Pointer on a function that adds a triangle to the user mesh
     *
     *  The argument \p triangle is the triangle to be added, note that
     *  struct gmio_stl_triangle::attribute_byte_count is meaningless for STL
     *  ascii.
     *
     *  The argument \p tri_id is the index of the mesh triangle
     */
    void (*func_add_triangle)(
            void* cookie,
            uint32_t tri_id,
            const struct gmio_stl_triangle* triangle);

    /*! Pointer on a function that finalizes creation of the user mesh
     *
     *  Optional function called at the end of the read process, ie. after all
     *  triangles have been added
     */
    void (*func_end_solid)(void* cookie);
};

#endif /* GMIO_STL_MESH_CREATOR_H */
/*! @} */
