/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

/*! \c GMIO_STL_FORMAT_TAG_BINARY
 *  Byte-mask to tag(identify) STL binary formats */
enum { GMIO_STL_FORMAT_TAG_BINARY = 0x10 };

/*! Various STL formats */
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
GMIO_API enum gmio_stl_format gmio_stl_format_probe(struct gmio_stream* stream);

/*! Returns the format of the STL data in file at location \p filepath
 *
 *  This function is a wrapper around gmio_stl_format_probe()
 *
 *  \param filepath Path to the STL file. A stream is opened with \c fopen() so
 *         the string shall follow the file name specifications of the running
 *         environment
 */
GMIO_API enum gmio_stl_format gmio_stl_format_probe_file(const char* filepath);

GMIO_C_LINKAGE_END

#endif /* GMIO_STL_FORMAT_H */
/*! @} */
