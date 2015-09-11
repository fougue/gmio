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

/*! Does binary STL header \p lhs compare equal to \p rhs ? */
GMIO_INLINE gmio_bool_t gmio_stlb_header_equal(
        const gmio_stlb_header_t* lhs, const gmio_stlb_header_t* rhs)
{
    return memcmp(lhs, rhs, GMIO_STLB_HEADER_SIZE) == 0;
}

/*! Callback for gmio_stl_mesh_creator::func_add_triangle that does
 *  nothing(ie "no operation") */
void gmio_stl_nop_add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* triangle);

/*! Holds an array of STL triangles */
struct gmio_stl_triangle_array
{
    gmio_stl_triangle_t* ptr;
    uint32_t count;
};
typedef struct gmio_stl_triangle_array gmio_stl_triangle_array_t;

/*! Holds complete STL data (usable for both binary and ascii formats) */
struct gmio_stl_data
{
    gmio_stlb_header_t header;
    gmio_stl_triangle_array_t tri_array;
};
typedef struct gmio_stl_data gmio_stl_data_t;

gmio_stl_mesh_creator_t gmio_stl_data_mesh_creator(gmio_stl_data_t* data);
gmio_stl_mesh_t gmio_stl_data_mesh(const gmio_stl_data_t* data);

#endif /* GMIO_TESTS_STL_UTILS_H */
