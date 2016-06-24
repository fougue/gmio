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

#ifndef GMIO_INTERNAL_NUMERIC_UTILS_H
#define GMIO_INTERNAL_NUMERIC_UTILS_H

#include "../global.h"
#include "convert.h"

#include <stddef.h>

/*! Does \p a and \p b compares equals by ULP (Units in the Last Place) ?
 *
 *  ULP = spacing between floating-point numbers
 *
 *  See:
 *    http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
 */
bool gmio_float32_ulp_equals(float a, float b, uint32_t max_ulp_diff);

/*! Count of ULP between \p a and \p b */
GMIO_INLINE uint32_t gmio_float32_ulp_diff(float a, float b);

/*! Portable sign-extraction for int32 */
GMIO_INLINE int gmio_int32_sign(int32_t v);

/*! Portable sign-extraction for float32 */
GMIO_INLINE int gmio_float32_sign(float v);



/*
 *  Implementation
 */

uint32_t gmio_float32_ulp_diff(float a, float b)
{
    const uint32_t ua = gmio_convert_uint32(a);
    const uint32_t ub = gmio_convert_uint32(b);
    return ua > ub ? ua - ub : ub - ua;
}

int gmio_int32_sign(int32_t v)
{
    return (v & 0x80000000) != 0 ? -1 : 1;
}

int gmio_float32_sign(float v)
{
    return gmio_int32_sign(gmio_convert_int32(v));
}

#endif /* GMIO_INTERNAL_NUMERIC_UTILS_H */
