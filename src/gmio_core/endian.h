/****************************************************************************
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
****************************************************************************/

/*! \file endian.h
 *  Architecture endianness
 */

#ifndef GMIO_ENDIAN_H
#define GMIO_ENDIAN_H

#include "global.h"

GMIO_C_LINKAGE_BEGIN

/*! This enum identifies common endianness (byte order) of computer memory */
enum gmio_endianness
{
    GMIO_LITTLE_ENDIAN,
    GMIO_BIG_ENDIAN,
    GMIO_MIDDLE_ENDIAN,
    GMIO_OTHER_ENDIAN
};

typedef enum gmio_endianness gmio_endianness_t;

/*! Returns endianness (byte order) of the host's CPU architecture */
GMIO_LIB_EXPORT gmio_endianness_t gmio_host_endianness();

GMIO_C_LINKAGE_END

#endif /* GMIO_ENDIAN_H */
