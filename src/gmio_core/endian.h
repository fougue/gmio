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
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

/*! \file endian.h
 *  Architecture endianness
 */

#ifndef GMIO_ENDIAN_H
#define GMIO_ENDIAN_H

#include "global.h"

/*! This enum identifies common endianness (byte order) of computer memory */
enum gmio_endianness
{
    /*! The least significant byte is stored at the lowest address. The other
     *  bytes follow in increasing order of significance */
    GMIO_LITTLE_ENDIAN,

    /*! The most significant byte is stored at the lowest address. The other
     *  bytes follow in decreasing order of significance */
    GMIO_BIG_ENDIAN,

    /*! Other (unknown) byte-order */
    GMIO_OTHER_ENDIAN,

#ifdef GMIO_HOST_IS_BIG_ENDIAN
    GMIO_HOST_ENDIANNESS = GMIO_BIG_ENDIAN
#else
    /*! Defines the endianness(byte order) used by the host computer for
     *  storing data in memory.
     *
     *  It is set at configure-time to either GMIO_LITTLE_ENDIAN or
     *  GMIO_BIG_ENDIAN
     */
    GMIO_HOST_ENDIANNESS = GMIO_LITTLE_ENDIAN
#endif
};

typedef enum gmio_endianness gmio_endianness_t;

GMIO_C_LINKAGE_BEGIN

/*! Returns endianness (byte order) of the host's CPU architecture */
GMIO_LIB_EXPORT gmio_endianness_t gmio_host_endianness();

GMIO_C_LINKAGE_END

#endif /* GMIO_ENDIAN_H */
