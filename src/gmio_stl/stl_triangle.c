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

#include "stl_triangle.h"

#include "../gmio_core/internal/vecgeom_utils.h"

GMIO_INLINE struct gmio_stl_coords gmio_stl_coords_diff(
        const struct gmio_stl_coords* u, const struct gmio_stl_coords* v)
{
    struct gmio_stl_coords diff;
    diff.x = u->x - v->x;
    diff.y = u->y - v->y;
    diff.z = u->z - v->z;
    return diff;
}

void gmio_stl_triangle_compute_normal(struct gmio_stl_triangle* tri)
{
    const struct gmio_stl_coords u = gmio_stl_coords_diff(&tri->v2, &tri->v1);
    const struct gmio_stl_coords v = gmio_stl_coords_diff(&tri->v3, &tri->v1);
    struct gmio_stl_coords* n = &tri->n;
    gmio_cross_product_f32(u.x, u.y, u.z, v.x, v.y, v.z, &n->x, &n->y, &n->z);
}
