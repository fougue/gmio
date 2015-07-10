/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougsys.fr
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

#ifndef GMIO_INTERNAL_HELPER_STL_MESH_CREATOR_H
#define GMIO_INTERNAL_HELPER_STL_MESH_CREATOR_H

#include "../stl_mesh_creator.h"

/*! Safe and convenient function for
 *  gmio_stl_mesh_creator::func_ascii_begin_solid() */
GMIO_INLINE void gmio_stl_mesh_creator_ascii_begin_solid(
        gmio_stl_mesh_creator_t* creator,
        size_t stream_size,
        const char* solid_name)
{
    if (creator != NULL && creator->func_ascii_begin_solid != NULL) {
        creator->func_ascii_begin_solid(
                    creator->cookie, stream_size, solid_name);
    }
}

/*! Safe and convenient function for
 *  gmio_stl_mesh_creator::func_binary_begin_solid() */
GMIO_INLINE void gmio_stl_mesh_creator_binary_begin_solid(
        gmio_stl_mesh_creator_t* creator,
        uint32_t tri_count,
        const gmio_stlb_header_t* header)
{
    if (creator != NULL && creator->func_binary_begin_solid != NULL)
        creator->func_binary_begin_solid(creator->cookie, tri_count, header);
}

/*! Safe and convenient function for
 *  gmio_stl_mesh_creator::func_add_triangle() */
GMIO_INLINE void gmio_stl_mesh_creator_add_triangle(
        gmio_stl_mesh_creator_t* creator,
        uint32_t tri_id,
        const gmio_stl_triangle_t* triangle)
{
    if (creator != NULL && creator->func_add_triangle != NULL)
        creator->func_add_triangle(creator->cookie, tri_id, triangle);
}

/*! Safe and convenient function for
 *  gmio_stl_mesh_creator::func_end_solid() */
GMIO_INLINE void gmio_stl_mesh_creator_end_solid(
        gmio_stl_mesh_creator_t* creator)
{
    if (creator != NULL && creator->func_end_solid != NULL)
        creator->func_end_solid(creator->cookie);
}

#endif /* GMIO_INTERNAL_HELPER_STL_MESH_CREATOR_H */
