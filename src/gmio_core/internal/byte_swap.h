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

#ifdef GMIO_HAVE_MSVC_BUILTIN_BSWAP
#  include <cstdlib>
#endif

namespace gmio {

//! Returns 'val' with the order of bytes reversed, uses compiler builtin
//! functions if available
inline uint16_t uint16_byteSwap(uint16_t val);

//! Returns 'val' with the order of bytes reversed, uses compiler builtin
//! functions if available
inline uint32_t uint32_byteSwap(uint32_t val);

//
// Implementation
//

uint16_t uint16_byteSwap(uint16_t val)
{
#ifdef GMIO_HAVE_GCC_BUILTIN_BSWAP16
    return __builtin_bswap16(val);
#elif defined(GMIO_HAVE_MSVC_BUILTIN_BSWAP)
    return _byteswap_ushort(val);
#else
    return ((val & 0x00FF) << 8) | ((val >> 8) & 0x00FF);
#endif
}

uint32_t uint32_byteSwap(uint32_t val)
{
#ifdef GMIO_HAVE_GCC_BUILTIN_BSWAP32
    return __builtin_bswap32(val);
#elif defined(GMIO_HAVE_MSVC_BUILTIN_BSWAP)
    return _byteswap_ulong(val);
#else
    return ((val & 0x000000FF) << 24)
            | ((val & 0x0000FF00) << 8)
            | ((val >> 8) & 0x0000FF00)
            | ((val >> 24) & 0x000000FF);
#endif
}

} // namespace gmio
