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

#include "../src/gmio_core/internal/min_max.h"
#include "../src/gmio_core/internal/numeric_utils.h"
#include "../src/gmio_core/internal/safe_cast.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void gmio_stl_nop_add_triangle(
        void *cookie, uint32_t tri_id, const struct gmio_stl_triangle *triangle)
{
    GMIO_UNUSED(cookie);
    GMIO_UNUSED(tri_id);
    GMIO_UNUSED(triangle);
}

struct gmio_stl_triangle_array gmio_stl_triangle_array_malloc(size_t tri_count)
{
    struct gmio_stl_triangle_array array = {0};
    if (tri_count > 0) {
        array.ptr =
                (struct gmio_stl_triangle*)calloc(
                    tri_count, sizeof(struct gmio_stl_triangle));
    }
    array.count = gmio_size_to_uint32(tri_count);
    array.capacity = array.count;
    return array;
}

static void gmio_stl_data__ascii_begin_solid(
        void* cookie, gmio_streamsize_t stream_size, const char* solid_name)
{
    struct gmio_stl_data* data = (struct gmio_stl_data*)cookie;

    memset(&data->solid_name[0], 0, sizeof(data->solid_name));
    if (solid_name != NULL) {
        const size_t len =
                GMIO_MIN(sizeof(data->solid_name), strlen(solid_name));
        strncpy(&data->solid_name[0], solid_name, len);
    }

    /* Try to guess how many vertices we could have assume we'll need 200 bytes
     * for each face */
    {
        const size_t facet_size = 200;
        const size_t facet_count =
                gmio_streamsize_to_size(GMIO_MAX(1, stream_size / facet_size));
        data->tri_array = gmio_stl_triangle_array_malloc(facet_count);
    }
}

static void gmio_stl_data__binary_begin_solid(
        void* cookie, uint32_t tri_count, const struct gmio_stlb_header* header)
{
    struct gmio_stl_data* data = (struct gmio_stl_data*)cookie;
    memcpy(&data->header, header, GMIO_STLB_HEADER_SIZE);
    data->tri_array = gmio_stl_triangle_array_malloc(tri_count);
}

static void gmio_stl_data__add_triangle(
        void* cookie, uint32_t tri_id, const struct gmio_stl_triangle* triangle)
{
    struct gmio_stl_data* data = (struct gmio_stl_data*)cookie;
    if (tri_id >= data->tri_array.capacity) {
        uint32_t cap = data->tri_array.capacity;
        cap += cap >> 3; /* Add 12.5% more capacity */
        data->tri_array.ptr =
                realloc(data->tri_array.ptr, cap * sizeof(struct gmio_stl_triangle));
        data->tri_array.capacity = cap;
    }
    memcpy(&data->tri_array.ptr[tri_id], triangle, GMIO_STLB_TRIANGLE_RAWSIZE);
    data->tri_array.count = GMIO_MAX(data->tri_array.count, tri_id + 1);
}

static void gmio_stl_data__get_triangle(
        const void* cookie, uint32_t tri_id, struct gmio_stl_triangle* triangle)
{
    const struct gmio_stl_data* data = (const struct gmio_stl_data*)cookie;
    *triangle = data->tri_array.ptr[tri_id];
}

struct gmio_stl_mesh_creator gmio_stl_data_mesh_creator(struct gmio_stl_data *data)
{
    struct gmio_stl_mesh_creator creator = {0};
    creator.cookie = data;
    creator.func_ascii_begin_solid = &gmio_stl_data__ascii_begin_solid;
    creator.func_binary_begin_solid = &gmio_stl_data__binary_begin_solid;
    creator.func_add_triangle = &gmio_stl_data__add_triangle;
    return creator;
}

struct gmio_stl_mesh gmio_stl_data_mesh(const struct gmio_stl_data *data)
{
    struct gmio_stl_mesh mesh = {0};
    mesh.cookie = data;
    mesh.triangle_count = data->tri_array.count;
    mesh.func_get_triangle = &gmio_stl_data__get_triangle;
    return mesh;
}

bool gmio_stl_coords_equal(
        const struct gmio_stl_coords *lhs,
        const struct gmio_stl_coords *rhs,
        uint32_t max_ulp_diff)
{
    return gmio_float32_ulp_equals(lhs->x, rhs->x, max_ulp_diff)
            && gmio_float32_ulp_equals(lhs->y, rhs->y, max_ulp_diff)
            && gmio_float32_ulp_equals(lhs->z, rhs->z, max_ulp_diff);
}

bool gmio_stl_triangle_equal(
        const struct gmio_stl_triangle *lhs,
        const struct gmio_stl_triangle *rhs,
        uint32_t max_ulp_diff)
{
    return gmio_stl_coords_equal(&lhs->n, &rhs->n, max_ulp_diff)
            && gmio_stl_coords_equal(&lhs->v1, &rhs->v1, max_ulp_diff)
            && gmio_stl_coords_equal(&lhs->v2, &rhs->v2, max_ulp_diff)
            && gmio_stl_coords_equal(&lhs->v3, &rhs->v3, max_ulp_diff)
            && lhs->attribute_byte_count == rhs->attribute_byte_count;
}

void gmio_stl_nop_get_triangle(
        const void *cookie, uint32_t tri_id, struct gmio_stl_triangle *triangle)
{
    GMIO_UNUSED(cookie);
    GMIO_UNUSED(tri_id);
    GMIO_UNUSED(triangle);
}
