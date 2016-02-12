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

/*! Cross product of float32 vectors (ux,uy,uz) and (vx,vy,vz) */
GMIO_INLINE void gmio_cross_product_f32(
        float ux, float uy, float uz,
        float vx, float vy, float vz,
        float* nx, float* ny, float* nz);

/*! Cross product of float64 vectors (ux,uy,uz) and (vx,vy,vz) */
GMIO_INLINE void gmio_cross_product_f64(
        double ux, double uy, double uz,
        double vx, double vy, double vz,
        double* nx, double* ny, double* nz);


/*
 *  Implementation
 */

#include <math.h>

void gmio_cross_product_f32(
        float ux, float uy, float uz,
        float vx, float vy, float vz,
        float* nx, float* ny, float* nz)
{
    *nx = uy*vz - uz*vx;
    *ny = uz*vx - ux*vz;
    *nz = ux*vy - uy*vx;
    {
        const float d = (float)sqrt((*nx)*(*nx) + (*ny)*(*ny) + (*nz)*(*nz));
        if (d > 0.f) {
            *nx /= d;
            *ny /= d;
            *nz /= d;
        }
    }
}

void gmio_cross_product_f64(
        double ux, double uy, double uz,
        double vx, double vy, double vz,
        double* nx, double* ny, double* nz)
{
    *nx = uy*vz - uz*vx;
    *ny = uz*vx - ux*vz;
    *nz = ux*vy - uy*vx;
    {
        const double d = sqrt((*nx)*(*nx) + (*ny)*(*ny) + (*nz)*(*nz));
        if (d > 0.) {
            *nx /= d;
            *ny /= d;
            *nz /= d;
        }
    }
}

#endif /* GMIO_INTERNAL_VECGEOM_UTILS_H */
