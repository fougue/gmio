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

GMIO_INLINE char* gmio_u32toa(uint32_t value, char* str);
GMIO_INLINE char* gmio_i32toa(int32_t value, char* str);
#ifdef GMIO_HAVE_INT64_TYPE
GMIO_INLINE char* gmio_u64toa(uint64_t value, char* str);
GMIO_INLINE char* gmio_i64toa(int64_t value, char* str);
#endif

/*
 * Implementation
 */

#include "../../3rdparty/miloyip_itoa/branchlut.h"

char* gmio_u32toa(uint32_t value, char* str)
{
    if (value < 10) {
        *str++ = '0' + (char)value;
        return str;
    }
    else {
        return u32toa_branchlut(value, str);
    }
}

char* gmio_i32toa(int32_t value, char* str)
{
    return i32toa_branchlut(value, str);
}

#ifdef GMIO_HAVE_INT64_TYPE
char* gmio_u64toa(uint64_t value, char* str)
{
    return u64toa_branchlut(value, str);
}

char* gmio_i64toa(int64_t value, char* str)
{
    return i64toa_branchlut(value, str);
}
#endif /* GMIO_HAVE_INT64_TYPE */
