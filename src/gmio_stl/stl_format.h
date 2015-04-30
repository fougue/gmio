/****************************************************************************
** GeomIO Library
** Copyright Fougue (2 Mar. 2015)
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

/*! \file stl_format.h
 *  Detection of STL format from input stream
 */

#ifndef GMIO_STL_FORMAT_H
#define GMIO_STL_FORMAT_H

#include "stl_global.h"
#include "../gmio_core/stream.h"

/*! This enums defines the various STL formats */
enum gmio_stl_format
{
    GMIO_STL_FORMAT_UNKNOWN = 0, /*!< Unknown STL format */
    GMIO_STL_FORMAT_ASCII,       /*!< STL ASCII (text) */
    GMIO_STL_FORMAT_BINARY_LE,   /*!< STL binary (little-endian) */
    GMIO_STL_FORMAT_BINARY_BE    /*!< STL binary (big-endian) */
};

typedef enum gmio_stl_format gmio_stl_format_t;

GMIO_C_LINKAGE_BEGIN

/*! Returns the format of the STL data in \p stream
 *
 *  It will try to read 512 bytes from \p stream into a buffer and then
 *  analyses this data to guess the format.
 *
 *  \retval GMIO_STL_FORMAT_UNKNOWN in case of error.
 */
GMIO_LIBSTL_EXPORT
gmio_stl_format_t gmio_stl_get_format(gmio_stream_t* stream);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_FORMAT_H */
