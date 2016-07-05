/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#ifndef GMIO_INTERNAL_HELPER_STL_MESH_CREATOR_H
#define GMIO_INTERNAL_HELPER_STL_MESH_CREATOR_H

#include "../stl_mesh_creator.h"

/*! Safe and convenient function for
 *  gmio_stl_mesh_creator::func_begin_solid() */
GMIO_INLINE void gmio_stl_mesh_creator_begin_solid(
        struct gmio_stl_mesh_creator* creator,
        const struct gmio_stl_mesh_creator_infos* infos)
{
    if (creator != NULL && creator->func_begin_solid != NULL)
        creator->func_begin_solid(creator->cookie, infos);
}

/*! Safe and convenient function for
 *  gmio_stl_mesh_creator::func_add_triangle() */
GMIO_INLINE void gmio_stl_mesh_creator_add_triangle(
        struct gmio_stl_mesh_creator* creator,
        uint32_t tri_id,
        const struct gmio_stl_triangle* triangle)
{
    if (creator != NULL && creator->func_add_triangle != NULL)
        creator->func_add_triangle(creator->cookie, tri_id, triangle);
}

/*! Safe and convenient function for
 *  gmio_stl_mesh_creator::func_end_solid() */
GMIO_INLINE void gmio_stl_mesh_creator_end_solid(
        struct gmio_stl_mesh_creator* creator)
{
    if (creator != NULL && creator->func_end_solid != NULL)
        creator->func_end_solid(creator->cookie);
}

#endif /* GMIO_INTERNAL_HELPER_STL_MESH_CREATOR_H */
