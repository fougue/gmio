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

/*! \file error.h
 *  List of common errors, reported by I/O functions
 *
 *  \addtogroup gmio_core
 *  @{
 */

#pragma once

#include "global.h"

/*! Common errors
 *
 *  Format: \n
 *    4-bytes signed integer (ISO C restricts enumerator value to 'int') \n
 *    max value: 0x7FFFFFFF \n
 *    bits 0x00FFFFFF..0x7FFFFFFF: tag identifying the error category \n
 *    bits 0x001FFFFF..0x00FFFFFF: tag identifying an error sub-category \n
 *    bits 0x00000000..0x000FFFFF: error value in the (sub)category \n
 *    Max count of categories : 128 (2^7) \n
 *    Max count of sub-categories : 15 (2^4 - 1) \n
 *    Max count of error values : 1048576 (2^20)
 */
enum gmio_error
{
    /*! No error occurred, success */
    GMIO_ERROR_OK = 0,

    /*! Unknown error */
    GMIO_ERROR_UNKNOWN,

    /*! Pointer on argument memory block is \c NULL */
    GMIO_ERROR_NULL_MEMBLOCK,

    /*! Argument size for the memory block is too small */
    GMIO_ERROR_INVALID_MEMBLOCK_SIZE,

    /*! An error occurred with gmio_stream */
    GMIO_ERROR_STREAM,

    /*! Task was stopped by user, that is to say
     *  gmio_task_iface::func_is_stop_requested() returned true */
    GMIO_ERROR_TASK_STOPPED,

    /*! An error occured after a call to a \c <stdio.h> function.
     *  The caller can check \c errno to get the real error number */
    GMIO_ERROR_STDIO,

    /*! Checking of \c LC_NUMERIC failed(should be "C" or "POSIX") */
    GMIO_ERROR_BAD_LC_NUMERIC,

    /* zlib */
    /*! See \c Z_ERRNO (file operation error) */
    GMIO_ERROR_ZLIB_ERRNO,

    /*! See \c Z_STREAM_ERROR */
    GMIO_ERROR_ZLIB_STREAM,

    /*! See \c Z_DATA_ERROR */
    GMIO_ERROR_ZLIB_DATA,

    /*! See \c Z_MEM_ERROR (not enough memory) */
    GMIO_ERROR_ZLIB_MEM,

    /*! See \c Z_BUF_ERROR */
    GMIO_ERROR_ZLIB_BUF,

    /*! See \c Z_VERSION_ERROR (zlib library version is incompatible with the
     *  version assumed by the caller) */
    GMIO_ERROR_ZLIB_VERSION,

    /*! Invalid compression level, see gmio_zlib_compress_options::level */
    GMIO_ERROR_ZLIB_INVALID_COMPRESS_LEVEL,

    /*! Invalid compression memory usage, see
     *  gmio_zlib_compress_options::memory_usage */
    GMIO_ERROR_ZLIB_INVALID_COMPRESS_MEMORY_USAGE,

    /* ZIP */
    /*! Zip64 format requires the compiler to provide a 64b integer type */
    GMIO_ERROR_ZIP_INT64_TYPE_REQUIRED,

    /*! The size of some ZIP file entry exceeds 32b limit and so requires Zip64
     *  format */
    GMIO_ERROR_ZIP64_FORMAT_REQUIRED
};

/*! \c GMIO_CORE_ERROR_TAG
 *  Byte-mask to tag(identify) gmio_core error codes */
enum { GMIO_CORE_ERROR_TAG = 0x00 };

/*! Returns true if <tt>code == GMIO_NO_ERROR</tt> */
GMIO_INLINE bool gmio_no_error(int code)
{ return code == GMIO_ERROR_OK ? true : false; }

/*! Returns true if <tt>code != GMIO_NO_ERROR</tt> */
GMIO_INLINE bool gmio_error(int code)
{ return code != GMIO_ERROR_OK ? true : false; }

/*! @} */
