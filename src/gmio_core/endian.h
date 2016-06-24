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

/*! \file endian.h
 *  Architecture endianness
 *
 *  \addtogroup gmio_core
 *  @{
 */

#ifndef GMIO_ENDIAN_H
#define GMIO_ENDIAN_H

#include "global.h"

/*! This enum identifies common endianness (byte order) of computer memory */
enum gmio_endianness
{
    /*! Other (unknown) byte-order */
    GMIO_ENDIANNESS_UNKNOWN = 0,

    /*! The least significant byte is stored at the lowest address. The other
     *  bytes follow in increasing order of significance */
    GMIO_ENDIANNESS_LITTLE,

    /*! The most significant byte is stored at the lowest address. The other
     *  bytes follow in decreasing order of significance */
    GMIO_ENDIANNESS_BIG,

#ifdef GMIO_HOST_IS_BIG_ENDIAN
    GMIO_ENDIANNESS_HOST = GMIO_ENDIANNESS_BIG
#else
    /*! Defines the endianness(byte order) used by the host computer for
     *  storing data in memory.
     *
     *  It is set at configure-time to either GMIO_ENDIANNESS_LITTLE or
     *  GMIO_ENDIANNESS_BIG
     */
    GMIO_ENDIANNESS_HOST = GMIO_ENDIANNESS_LITTLE
#endif
};

GMIO_C_LINKAGE_BEGIN

/*! Returns endianness (byte order) of the host's CPU architecture */
GMIO_API enum gmio_endianness gmio_host_endianness();

GMIO_C_LINKAGE_END

#endif /* GMIO_ENDIAN_H */
/*! @} */
