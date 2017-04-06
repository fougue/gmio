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
