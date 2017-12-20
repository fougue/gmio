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

namespace gmio {

//! Common errors
//!
//! Format: \n
//!   4-bytes signed integer (ISO C restricts enumerator value to 'int') \n
//!   max value: 0x7FFFFFFF \n
//!   bits 0x00FFFFFF..0x7FFFFFFF: tag identifying the error category \n
//!   bits 0x001FFFFF..0x00FFFFFF: tag identifying an error sub-category \n
//!   bits 0x00000000..0x000FFFFF: error value in the (sub)category \n
//!   Max count of categories : 128 (2^7) \n
//!   Max count of sub-categories : 15 (2^4 - 1) \n
//!   Max count of error values : 1048576 (2^20)
enum Error {
    //! No error occurred, success
    Error_OK = 0,

    //! Unknown error
    Error_Unknown,

    //! An error occurred with gmio_stream
    Error_Stream,

    //! Task was stopped by user, that is to say Task::isStopRequested()
    //! returned true
    Error_TaskStopped,

    //! An error occured after a call to a \c <stdio.h> function.
    //! The caller can check \c errno to get the real error number
    Error_Stdio,

    //! Checking of \c LC_NUMERIC failed(should be "C" or "POSIX")
    Error_BadLcNumeric,

    Error_BufferOverflow,

    /* zlib */
    //! See \c Z_ERRNO (file operation error)
    Error_Zlib_Errno,

    //! See \c Z_STREAM_ERROR
    Error_Zlib_Stream,

    //! See \c Z_DATA_ERROR
    Error_Zlib_Data,

    //! See \c Z_MEM_ERROR (not enough memory)
    Error_Zlib_Mem,

    //! See \c Z_BUF_ERROR
    Error_ZLib_Buf,

    //! See \c Z_VERSION_ERROR (zlib library version is incompatible with the
    //! version assumed by the caller)
    Error_Zlib_Version,

    //! Invalid compression memory usage, see
    //! gmio_zlib_compress_options::memory_usage
    Error_Zlib_InvalidCompressMemoryUsage,

    //! All input to be deflated(compressed) was not processed
    Error_Zlib_DeflateNotAllInputUsed,

    //! Deflate failure to flush pending output
    Error_Zlib_DeflateStreamIncomplete,

    /* ZIP */
    //! The size of some ZIP file entry exceeds 32b limit and so requires Zip64 format
    Error_Zip64_FormatRequired
};

//!  Byte-mask to tag(identify) core error codes
const unsigned TagError = 0x00;

//! Returns true if <tt>code == Error_OK</tt>
constexpr bool noError(int code)
{ return code == Error_OK; }

//! Returns true if <tt>code != Error_OK</tt>
constexpr bool error(int code)
{ return code != Error_OK; }

} // namespace gmio

//! @} */
