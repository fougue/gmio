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

#ifndef GMIO_INTERNAL_SAFE_CAST_H
#define GMIO_INTERNAL_SAFE_CAST_H

#include "../global.h"

#include <stddef.h>

/*! Returns \p val safely casted to unsigned 32b integer */
GMIO_INLINE uint32_t gmio_size_to_uint32(size_t val)
{
#if GMIO_TARGET_ARCH_BIT_SIZE > 32
    return val > 0xFFFFFFFF ? 0xFFFFFFFF : (uint32_t)val;
#else
    return val;
#endif
}

#endif /* GMIO_INTERNAL_SAFE_CAST_H */
