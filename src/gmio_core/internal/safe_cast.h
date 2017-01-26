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

#ifndef GMIO_INTERNAL_SAFE_CAST_H
#define GMIO_INTERNAL_SAFE_CAST_H

#include "../global.h"
#include "../stream.h"

#include <stddef.h>


/*! Returns \p val safely casted to unsigned 32b integer */
GMIO_INLINE uint32_t gmio_size_to_uint32(size_t val)
{
#if GMIO_TARGET_ARCH_BIT_SIZE > 32
    /* TODO : eliminate branch */
    return val > 0xFFFFFFFF ? 0xFFFFFFFF : (uint32_t)val;
#else
    return val;
#endif
}

#define GMIO_MAX_SIZET  ((size_t)-1)

/*! Returns \p val safely casted to \c size_t */
GMIO_INLINE size_t gmio_streamsize_to_size(gmio_streamsize_t val)
{
#if GMIO_TARGET_ARCH_BIT_SIZE < 64 \
    && defined(GMIO_HAVE_INT64_TYPE)
    /* TODO : eliminate branch */
    const uint64_t uval = val;
    return uval > GMIO_MAX_SIZET ? GMIO_MAX_SIZET : (size_t)uval;
#else
    return (size_t)val;
#endif
}

#endif /* GMIO_INTERNAL_SAFE_CAST_H */
