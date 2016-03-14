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

#ifndef GMIO_INTERNAL_C99_MATH_COMPAT_H
#define GMIO_INTERNAL_C99_MATH_COMPAT_H

#include "../global.h"

#include <math.h>

#ifdef GMIO_HAVE_POWF_FUNC
#  define gmio_powf powf
#else
/* No powf() function, call pow(double) as fallback */
GMIO_INLINE float gmio_powf(float base, float exponent)
{ return (float)pow((double)base, (double)exponent); }
#endif

#ifdef GMIO_HAVE_SQRTF_FUNC
#  define gmio_sqrtf sqrtf
#else
/* No sqrtf() function, call sqrt(double) as fallback */
GMIO_INLINE float gmio_sqrtf(float x)
{ return (float)sqrt((double)x); }
#endif

#endif /* GMIO_INTERNAL_C99_MATH_COMPAT_H */
