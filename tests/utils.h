/****************************************************************************
** GeomIO Library
** Copyright Fougue (2 Mar. 2015)
** contact@fougsys.fr
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

#ifndef GMIO_TESTS_UTILS_H
#define GMIO_TESTS_UTILS_H

#include "../src/gmio_core/global.h"
#include "../src/gmio_core/internal/convert.h"

/*! Does \p a and \p b compares equals by ULP (Units in the Last Place) ?
 *
 *  ULP = spacing between floating-point numbers
 *
 *  See:
 *    http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
 */
gmio_bool_t gmio_float32_equals_by_ulp(
        gmio_float32_t a, gmio_float32_t b, uint32_t max_ulp_diff);

/*! Count of ULP between \p a and \p b */
uint32_t gmio_float32_ulp_diff(gmio_float32_t a, gmio_float32_t b);

/*! Portable sign-extraction for int32 */
GMIO_INLINE int gmio_int32_sign(int32_t v)
{
    return (v & 0x80000000) != 0 ? -1 : 1;
}

/*! Portable sign-extraction for float32 */
GMIO_INLINE int gmio_float32_sign(gmio_float32_t v)
{
    return gmio_int32_sign(gmio_convert_int32(v));
}

#endif /* GMIO_TESTS_UTILS_H */
