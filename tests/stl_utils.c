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

#include "stl_utils.h"

#include <stdlib.h>

void gmio_stl_nop_add_triangle(
        void *cookie, uint32_t tri_id, const gmio_stl_triangle_t *triangle)
{
    GMIO_UNUSED(cookie);
    GMIO_UNUSED(tri_id);
    GMIO_UNUSED(triangle);
}

static void gmio_stl_data__binary_begin_solid(
        void* cookie, uint32_t tri_count, const gmio_stlb_header_t* header)
{
    gmio_stl_data_t* data = (gmio_stl_data_t*)cookie;
    memcpy(&data->header, header, GMIO_STLB_HEADER_SIZE);
    data->tri_array.count = tri_count;
    if (tri_count > 0) {
        data->tri_array.ptr =
                (gmio_stl_triangle_t*)malloc(tri_count * sizeof(gmio_stl_triangle_t));
    }
}

static void gmio_stl_data__add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* triangle)
{
    gmio_stl_data_t* data = (gmio_stl_data_t*)cookie;
    data->tri_array.ptr[tri_id] = *triangle;
}

static void gmio_stl_data__get_triangle(
        const void* cookie, uint32_t tri_id, gmio_stl_triangle_t* triangle)
{
    const gmio_stl_data_t* data = (const gmio_stl_data_t*)cookie;
    *triangle = data->tri_array.ptr[tri_id];
}

gmio_stl_mesh_creator_t gmio_stl_data_mesh_creator(gmio_stl_data_t *data)
{
    gmio_stl_mesh_creator_t creator = {0};
    creator.cookie = data;
    creator.func_binary_begin_solid = &gmio_stl_data__binary_begin_solid;
    creator.func_add_triangle = &gmio_stl_data__add_triangle;
    return creator;
}

gmio_stl_mesh_t gmio_stl_data_mesh(const gmio_stl_data_t *data)
{
    gmio_stl_mesh_t mesh = {0};
    mesh.cookie = data;
    mesh.triangle_count = data->tri_array.count;
    mesh.func_get_triangle = &gmio_stl_data__get_triangle;
    return mesh;
}
