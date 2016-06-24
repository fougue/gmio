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
