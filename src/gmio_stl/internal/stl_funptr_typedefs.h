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

#ifndef GMIO_INTERNAL_STL_FUNPTR_TYPEDEFS_H
#define GMIO_INTERNAL_STL_FUNPTR_TYPEDEFS_H

#include <stddef.h>

#include "../stl_mesh.h"
#include "../stl_mesh_creator.h"

/* gmio_stl_triangle */
typedef void (*gmio_stl_triangle_func_fix_endian_t)(struct gmio_stl_triangle*);

/* gmio_stl_mesh */
typedef void (*gmio_stl_mesh_func_get_triangle_t)(
        const void*, uint32_t, struct gmio_stl_triangle*);

/* gmio_stl_mesh_creator */
typedef void (*gmio_stl_mesh_creator_func_ascii_begin_solid_t)(
        void*, gmio_streamsize_t, const char*);
typedef void (*gmio_stl_mesh_creator_func_binary_begin_solid_t)(
        void*, uint32_t, const struct gmio_stlb_header*);
typedef void (*gmio_stl_mesh_creator_func_add_triangle_t)(
        void*, uint32_t, const struct gmio_stl_triangle*);
typedef void (*gmio_stl_mesh_creator_func_end_solid_t)(void*);

#endif /* GMIO_INTERNAL_STL_FUNPTR_TYPEDEFS_H */
