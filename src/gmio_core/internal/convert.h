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

#ifndef GMIO_INTERNAL_CONVERT_H
#define GMIO_INTERNAL_CONVERT_H

#include "../global.h"

union gmio_uint_float_32
{
    uint32_t       as_uint32;
    gmio_float32_t as_float32;
};

union gmio_int_float_32
{
    int32_t        as_int32;
    gmio_float32_t as_float32;
};

GMIO_INLINE gmio_float32_t gmio_convert_ufloat32(uint32_t val)
{
    union gmio_uint_float_32 conv;
    conv.as_uint32 = val;
    return conv.as_float32;
}

GMIO_INLINE uint32_t gmio_convert_uint32(gmio_float32_t val)
{
    union gmio_uint_float_32 conv;
    conv.as_float32 = val;
    return conv.as_uint32;
}

GMIO_INLINE gmio_float32_t gmio_convert_sfloat32(int32_t val)
{
    union gmio_int_float_32 conv;
    conv.as_int32 = val;
    return conv.as_float32;
}

GMIO_INLINE int32_t gmio_convert_int32(gmio_float32_t val)
{
    union gmio_int_float_32 conv;
    conv.as_float32 = val;
    return conv.as_int32;
}

#endif /* GMIO_INTERNAL_CONVERT_H */
