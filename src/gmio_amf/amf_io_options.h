/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
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

/*! \file amf_io_options.h
 *  Options for AMF read/write functions
 *
 *  \addtogroup gmio_amf
 *  @{
 */

#pragma once

#include "amf_global.h"
#include "../gmio_core/memblock.h"
#include "../gmio_core/task_iface.h"
#include "../gmio_core/text_format.h"
#include "../gmio_core/zlib_compress.h"

/*! Options of function gmio_amf_write()
 *
 *  Initialising gmio_amf_write_options with \c {0} (or \c {} in C++) is the
 *  convenient way to set default values(passing \c NULL to gmio_amf_write() has
 *  the same effect).
 */
struct gmio_amf_write_options
{
    /*! Used by the stream to bufferize I/O operations.
     *  If null, then a temporary memblock is created with the global default
     *  constructor function
     *  \sa gmio_memblock_isnull()
     *  \sa gmio_memblock_default() */
    struct gmio_memblock stream_memblock;

    /*! Optional interface by which the I/O operation can be controlled */
    struct gmio_task_iface task_iface;

    /*! The format used when writting double values as strings.
     *  Defaults to \c GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE when calling
     *  gmio_amf_write() with \c options==NULL */
    enum gmio_float_text_format float64_format;

    /*! The maximum number of significant digits when writting \c double values.
     *  Defaults to \c 16 when calling gmio_amf_write() with \c options==NULL */
    uint8_t float64_prec;

    /* ZIP/Deflate compression */

    /*! Flag to write AMF geometry in a ZIP archive containing one file entry.
     *  Options below have no effect if <tt>create_zip_archive==false</tt> */
    bool create_zip_archive;

    /*! Filename of the single AMF entry within the ZIP archive */
    const char* zip_entry_filename;

    /*! Filename length of the single AMF entry within the ZIP archive */
    uint16_t zip_entry_filename_len;

    /*! Flag to disable use of the Zip64 format extensions */
    bool dont_use_zip64_extensions;

    /*! Options for the zlib(deflate) compression */
    struct gmio_zlib_compress_options z_compress_options;
};

/*! @} */
