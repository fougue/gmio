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

#ifndef GMIO_INTERNAL_C99_STDIO_COMPAT_H
#define GMIO_INTERNAL_C99_STDIO_COMPAT_H

#include "../global.h"

#include <stddef.h>
#include <stdio.h>

/* Note:
 *     Visual C++ provides snprintf() since version 2015, before that it was
 *     used to provide only equivalent _snprintf()
 *
 *     snprintf() appears in C99
 */

#ifdef GMIO_HAVE_SNPRINTF_FUNC
#  define gmio_snprintf snprintf
#elif defined(GMIO_HAVE_WIN__SNPRINTF_FUNC)
#  define gmio_snprintf _snprintf
#else
#  include <stdarg.h>
/* No existing snprintf()-like function, call unsafe vsprintf() as fallback */
GMIO_INLINE int gmio_snprintf(char* buf, size_t bufn, const char* fmt, ...)
{
    int ret = 0;
    va_list args;
    va_start(args, fmt);
    ret = vsprintf(buf, fmt, args);
    va_end(args);
    return ret;
}
#endif

#endif /* GMIO_INTERNAL_C99_STDIO_COMPAT_H */
