/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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

GMIO_INLINE struct gmio_vec3f gmio_vec3f_sub(
        const struct gmio_vec3f* u, const struct gmio_vec3f* v)
{
    struct gmio_vec3f diff;
    diff.x = u->x - v->x;
    diff.y = u->y - v->y;
    diff.z = u->z - v->z;
    return diff;
}

void gmio_stl_triangle_compute_normal(struct gmio_stl_triangle* tri)
{
    const struct gmio_vec3f u = gmio_vec3f_sub(&tri->v2, &tri->v1);
    const struct gmio_vec3f v = gmio_vec3f_sub(&tri->v3, &tri->v1);
    gmio_vec3f_cross_product(&u, &v, &tri->n);
}
