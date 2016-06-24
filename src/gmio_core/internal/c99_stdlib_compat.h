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

#ifndef GMIO_INTERNAL_C99_STDLIB_COMPAT_H
#define GMIO_INTERNAL_C99_STDLIB_COMPAT_H

#include "../global.h"

#include <stdlib.h>

#ifdef GMIO_HAVE_STRTOF_FUNC
#  define gmio_strtof strtof
#else
/* No strtof() function, call strtod() as fallback */
GMIO_INLINE float gmio_strtof(const char* str, char** endptr)
{ return (float)strtod(str, endptr); }
#endif

#endif /* GMIO_INTERNAL_C99_STDLIB_COMPAT_H */
