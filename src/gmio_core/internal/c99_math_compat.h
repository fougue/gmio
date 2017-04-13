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

#pragma once

#include "../global.h"
#include <math.h>

#ifdef GMIO_HAVE_POWF
#  define gmio_powf powf
#else
/* No powf() function, call pow(double) as fallback */
GMIO_INLINE float gmio_powf(float base, float exponent)
{ return (float)pow((double)base, (double)exponent); }
#endif

#ifdef GMIO_HAVE_SQRTF
#  define gmio_sqrtf sqrtf
#else
/* No sqrtf() function, call sqrt(double) as fallback */
GMIO_INLINE float gmio_sqrtf(float x)
{ return (float)sqrt((double)x); }
#endif

#if defined(GMIO_HAVE_ISFINITE)
#  define gmio_isfinite(x) isfinite(x)
#elif defined(GMIO_HAVE_WIN__FINITE)
#  include <float.h>
#  define gmio_isfinite(x) _finite(x)
#else
/* No isfinite() symbol */
#  define gmio_isfinite(x) (((x) != NAN) && ((x) != INFINITY))
#endif

#if defined(GMIO_HAVE_ISNAN)
#  define gmio_isnan(x) isnan(x)
#elif defined(GMIO_HAVE_WIN__ISNAN)
#  include <float.h>
#  define gmio_isnan(x) _isnan(x)
#else
/* No isnan() symbol */
#  define gmio_isnan(x) ((x) == NAN)
#endif
