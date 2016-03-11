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

/*! \file stl_format.h
 *  Detection of STL format from input stream
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_FORMAT_H
#define GMIO_STL_FORMAT_H

#include "stl_global.h"
#include "../gmio_core/stream.h"

/*! A byte-mask to tag(identify) STL binary formats */
enum { GMIO_STL_FORMAT_TAG_BINARY = 0x10 };

/*! This enums defines the various STL formats */
enum gmio_stl_format
{
    /*! Unknown STL format */
    GMIO_STL_FORMAT_UNKNOWN = 0,
    /*! STL ASCII (text) */
    GMIO_STL_FORMAT_ASCII = 0x01,
    /*! STL binary (little-endian) */
    GMIO_STL_FORMAT_BINARY_LE = GMIO_STL_FORMAT_TAG_BINARY | 0x01,
    /*! STL binary (big-endian) */
    GMIO_STL_FORMAT_BINARY_BE = GMIO_STL_FORMAT_TAG_BINARY | 0x02
};

GMIO_C_LINKAGE_BEGIN

/*! Returns the format of the STL data in \p stream
 *
 *  It will try to read 512 bytes from \p stream into a buffer and then
 *  analyses this data to guess the format.
 *
 *  The position of the input stream is preserved.
 *
 *  \retval GMIO_STL_FORMAT_UNKNOWN in case of error.
 */
GMIO_LIBSTL_EXPORT
enum gmio_stl_format gmio_stl_get_format(struct gmio_stream* stream);

/*! Returns the format of the STL data in file at location \p filepath
 *
 *  This function is a wrapper around gmio_stl_get_format()
 *
 *  \param filepath Path to the STL file. A stream is opened with fopen() so
 *         the string shall follow the file name specifications of the running
 *         environment
 */
GMIO_LIBSTL_EXPORT
enum gmio_stl_format gmio_stl_get_format_file(const char* filepath);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_FORMAT_H */
/*! @} */
