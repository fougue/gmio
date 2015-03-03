/****************************************************************************
**
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
**
****************************************************************************/

#ifndef GMIO_INTERNAL_CONVERT_H
#define GMIO_INTERNAL_CONVERT_H

#include "../global.h"

union gmio_uint_float_32
{
    uint32_t       as_uint32;
    gmio_float32_t as_float32;
};
typedef union gmio_uint_float_32  gmio_uint_float_32_t;

gmio_float32_t gmio_convert_float32(uint32_t val);
uint32_t       gmio_convert_uint32(gmio_float32_t val);

#endif /* GMIO_INTERNAL_CONVERT_H */
