/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
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

#pragma once

#include "../src/gmio_core/global.h"
#include "../src/gmio_stl/stl_mesh.h"
#include "../src/gmio_stl/stl_mesh_creator.h"
#include "../src/gmio_stl/stl_triangle.h"
#include "../src/gmio_stl/stlb_header.h"

#include <string.h>

bool gmio_stl_triangle_equal(
        const struct gmio_stl_triangle* lhs,
        const struct gmio_stl_triangle* rhs,
        uint32_t max_ulp_diff);

/*! Does binary STL header \p lhs compare equal to \p rhs ? */
GMIO_INLINE bool gmio_stlb_header_equal(
        const struct gmio_stlb_header* lhs, const struct gmio_stlb_header* rhs)
{
    return memcmp(lhs, rhs, GMIO_STLB_HEADER_SIZE) == 0;
}

/*! Callback for gmio_stl_mesh_creator::func_add_triangle that does
 *  nothing(ie "no operation") */
void gmio_stl_nop_add_triangle(
        void* cookie, uint32_t tri_id, const struct gmio_stl_triangle* triangle);

/*! Callback for gmio_stl_mesh::func_get_triangle that does nothing */
void gmio_stl_nop_get_triangle(
        const void* cookie, uint32_t tri_id, struct gmio_stl_triangle* triangle);

/*! Holds an array of STL triangles */
struct gmio_stl_triangle_array
{
    struct gmio_stl_triangle* ptr;
    uint32_t count;
    uint32_t capacity;
};

/*! Returns an dynamically allocated array of struct gmio_stl_triangle
 *
 *  Contents of the memory block beginnning at gmio_stl_triangle_array::ptr
 *  is initialized with zeroes
 */
struct gmio_stl_triangle_array gmio_stl_triangle_array_malloc(size_t tri_count);

void gmio_stl_triangle_array_free(struct gmio_stl_triangle_array* array);

/*! Holds complete STL data (usable for both binary and ascii formats) */
struct gmio_stl_data
{
    struct gmio_stlb_header header;
    char solid_name[1024];
    struct gmio_stl_triangle_array tri_array;
};

struct gmio_stl_mesh_creator gmio_stl_data_mesh_creator(struct gmio_stl_data* data);
struct gmio_stl_mesh gmio_stl_data_mesh(const struct gmio_stl_data* data);
