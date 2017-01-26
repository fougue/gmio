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
#include <float.h>

void gmio_vec3f_cross_product(
        const struct gmio_vec3f* u,
        const struct gmio_vec3f* v,
        struct gmio_vec3f* n)
{
    const float ux = u->x, uy = u->y, uz = u->z;
    const float vx = v->x, vy = v->y, vz = v->z;
    n->x = uy*vz - uz*vy;
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
    n->x = uy*vz - uz*vy;
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
    if (d > FLT_MIN) {
        v->x /= d;
        v->y /= d;
        v->z /= d;
    }
}

void gmio_vec3d_normalize(struct gmio_vec3d* v)
{
    const double d = sqrt(gmio_vec3d_sqr_length(v));
    if (d > DBL_MIN) {
        v->x /= d;
        v->y /= d;
        v->z /= d;
    }
}

#endif /* GMIO_INTERNAL_VECGEOM_UTILS_H */
