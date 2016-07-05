/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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

#ifndef GMIO_INTERNAL_C99_STDIO_COMPAT_H
#define GMIO_INTERNAL_C99_STDIO_COMPAT_H

#include "../global.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

/* Note:
 *     Visual C++ provides vsnprintf() since version 2005, before it used to
 *     provide only _vsnprintf()
 *
 *     vsnprintf() appeared in C99
 */
#ifdef GMIO_HAVE_VSNPRINTF_FUNC
#  define gmio_vsnprintf vsnprintf
#elif defined(GMIO_HAVE_WIN__VSNPRINTF_FUNC)
#  define gmio_vsnprintf _vsnprintf
#else
/* No vsnprintf()-like function, call unsafe vsprintf() as fallback */
GMIO_INLINE int gmio_vsnprintf(
        char* buf, size_t bufn, const char* fmt, va_list args)
{
    GMIO_UNUSED(bufn);
    return vsprintf(buf, fmt, args);
}
#endif

/* Note:
 *     Visual C++ provides snprintf() since version 2015, before it used to
 *     provide only _snprintf()
 *
 *     snprintf() appeared in C99
 */
#ifdef GMIO_HAVE_SNPRINTF_FUNC
#  define gmio_snprintf snprintf
#elif defined(GMIO_HAVE_WIN__SNPRINTF_FUNC)
#  define gmio_snprintf _snprintf
#else
/* No snprintf()-like function, translate to gmio_vsnprintf() call */
GMIO_INLINE int gmio_snprintf(char* buf, size_t bufn, const char* fmt, ...)
{
    int ret = 0;
    va_list args;
    va_start(args, fmt);
    ret = gmio_vsnprintf(buf, bufn, fmt, args);
    va_end(args);
    return ret;
}
#endif

#endif /* GMIO_INTERNAL_C99_STDIO_COMPAT_H */
