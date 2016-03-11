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

/*! Computes in-place cross product of float32 coords (ux,uy,uz) ^ (vx,vy,vz) */
GMIO_INLINE void gmio_cross_product_f32(
        float ux, float uy, float uz,
        float vx, float vy, float vz,
        float* nx, float* ny, float* nz);

/*! Computes in-place cross product of float64 coords (ux,uy,uz) ^ (vx,vy,vz) */
GMIO_INLINE void gmio_cross_product_f64(
        double ux, double uy, double uz,
        double vx, double vy, double vz,
        double* nx, double* ny, double* nz);

/*! Returns the squared length of float32 vector (x, y, z) */
GMIO_INLINE float gmio_sqr_length_f32(float x, float y, float z);

/*! Returns the squared length of float64 vector (x, y, z) */
GMIO_INLINE double gmio_sqr_length_f64(double x, double y, double z);

/*! Normalizes in-place the float32 (x,y,z) coords */
GMIO_INLINE void gmio_normalize_f32(float* x, float* y, float* z);

/*! Normalizes in-place the float64 (x,y,z) coords */
GMIO_INLINE void gmio_normalize_f64(double* x, double* y, double* z);

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
    gmio_normalize_f32(nx, ny, nz);
}

void gmio_cross_product_f64(
        double ux, double uy, double uz,
        double vx, double vy, double vz,
        double* nx, double* ny, double* nz)
{
    *nx = uy*vz - uz*vx;
    *ny = uz*vx - ux*vz;
    *nz = ux*vy - uy*vx;
    gmio_normalize_f64(nx, ny, nz);
}

float gmio_sqr_length_f32(float x, float y, float z)
{ return x*x + y*y + z*z; }

double gmio_sqr_length_f64(double x, double y, double z)
{ return x*x + y*y + z*z; }

void gmio_normalize_f32(float* x, float* y, float* z)
{
    const float d = (float)sqrt(gmio_sqr_length_f32(*x, *y, *z));
    if (d > 0.f) {
        *x /= d;
        *y /= d;
        *z /= d;
    }
}

void gmio_normalize_f64(double* x, double* y, double* z)
{
    const double d = sqrt(gmio_sqr_length_f64(*x, *y, *z));
    if (d > 0.) {
        *x /= d;
        *y /= d;
        *z /= d;
    }
}

#endif /* GMIO_INTERNAL_VECGEOM_UTILS_H */
