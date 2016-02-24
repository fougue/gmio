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

#ifndef GMIO_TESTS_STL_UTILS_H
#define GMIO_TESTS_STL_UTILS_H

#include "../src/gmio_core/global.h"
#include "../src/gmio_stl/stl_mesh.h"
#include "../src/gmio_stl/stl_mesh_creator.h"
#include "../src/gmio_stl/stl_triangle.h"
#include "../src/gmio_stl/stlb_header.h"

#include <string.h>

bool gmio_stl_coords_equal(
        const struct gmio_stl_coords* lhs,
        const struct gmio_stl_coords* rhs,
        uint32_t max_ulp_diff);

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

#endif /* GMIO_TESTS_STL_UTILS_H */
