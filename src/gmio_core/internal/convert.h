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

#ifndef GMIO_INTERNAL_CONVERT_H
#define GMIO_INTERNAL_CONVERT_H

#include "../global.h"

union gmio_uint_float_32
{
    uint32_t as_uint32;
    float    as_float32;
};

union gmio_int_float_32
{
    int32_t as_int32;
    float   as_float32;
};

GMIO_INLINE float gmio_convert_ufloat32(uint32_t val)
{
    union gmio_uint_float_32 conv;
    conv.as_uint32 = val;
    return conv.as_float32;
}

GMIO_INLINE uint32_t gmio_convert_uint32(float val)
{
    union gmio_uint_float_32 conv;
    conv.as_float32 = val;
    return conv.as_uint32;
}

GMIO_INLINE float gmio_convert_sfloat32(int32_t val)
{
    union gmio_int_float_32 conv;
    conv.as_int32 = val;
    return conv.as_float32;
}

GMIO_INLINE int32_t gmio_convert_int32(float val)
{
    union gmio_int_float_32 conv;
    conv.as_float32 = val;
    return conv.as_int32;
}

#endif /* GMIO_INTERNAL_CONVERT_H */
