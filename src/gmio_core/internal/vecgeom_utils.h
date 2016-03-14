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

#ifndef GMIO_INTERNAL_VECGEOM_UTILS_H
#define GMIO_INTERNAL_VECGEOM_UTILS_H

#include "../global.h"

/*! Computes in-place cross product of two gmio_vec3f objects */
GMIO_INLINE void gmio_vec3f_cross_product(
        const struct gmio_vec3f* u,
        const struct gmio_vec3f* v,
        struct gmio_vec3f* n);

/*! Computes in-place cross product of two gmio_vec3d objects */
GMIO_INLINE void gmio_vec3d_cross_product(
        const struct gmio_vec3d* u,
        const struct gmio_vec3d* v,
        struct gmio_vec3d* n);

/*! Returns the squared length of a gmio_vec3f object */
GMIO_INLINE float gmio_vec3f_sqr_length(const struct gmio_vec3f* v);

/*! Returns the squared length of a gmio_vec3d object */
GMIO_INLINE double gmio_vec3d_sqr_length(const struct gmio_vec3d* v);

/*! Normalizes in-place a gmio_vec3f object */
GMIO_INLINE void gmio_vec3f_normalize(struct gmio_vec3f* v);

/*! Normalizes in-place a gmio_vec3d object */
GMIO_INLINE void gmio_vec3d_normalize(struct gmio_vec3d* v);

/*
 *  Implementation
 */

#include "c99_math_compat.h"

void gmio_vec3f_cross_product(
        const struct gmio_vec3f* u,
        const struct gmio_vec3f* v,
        struct gmio_vec3f* n)
{
    const float ux = u->x, uy = u->y, uz = u->z;
    const float vx = v->x, vy = v->y, vz = v->z;
    n->x = uy*vz - uz*vx;
    n->y = uz*vx - ux*vz;
    n->z = ux*vy - uy*vx;
    gmio_vec3f_normalize(n);
}

void gmio_vec3d_cross_product(
        const struct gmio_vec3d* u,
        const struct gmio_vec3d* v,
        struct gmio_vec3d* n)
{
    const double ux = u->x, uy = u->y, uz = u->z;
    const double vx = v->x, vy = v->y, vz = v->z;
    n->x = uy*vz - uz*vx;
    n->y = uz*vx - ux*vz;
    n->z = ux*vy - uy*vx;
    gmio_vec3d_normalize(n);
}

float gmio_vec3f_sqr_length(const struct gmio_vec3f* v)
{
    const float vx = v->x, vy = v->y, vz = v->z;
    return vx*vx + vy*vy + vz*vz;
}

double gmio_vec3d_sqr_length(const struct gmio_vec3d* v)
{
    const double vx = v->x, vy = v->y, vz = v->z;
    return vx*vx + vy*vy + vz*vz;
}

void gmio_vec3f_normalize(struct gmio_vec3f* v)
{
    const float d = gmio_sqrtf(gmio_vec3f_sqr_length(v));
    if (d > 0.f) {
        v->x /= d;
        v->y /= d;
        v->z /= d;
    }
}

void gmio_vec3d_normalize(struct gmio_vec3d* v)
{
    const double d = sqrt(gmio_vec3d_sqr_length(v));
    if (d > 0.) {
        v->x /= d;
        v->y /= d;
        v->z /= d;
    }
}

#endif /* GMIO_INTERNAL_VECGEOM_UTILS_H */
