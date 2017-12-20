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

/*! \file zlib_compress.h
 *  Options for using compression with zlib
 *
 *  \addtogroup gmio_core
 *  @{
 */

#pragma once

#include "global.h"

namespace gmio {

//! zlib compression level, specific values
enum class ZLIB_CompressLevel {
    None = -1,   //!<  -> Z_NO_COMPRESSION
    Default = 0, //!<  -> Z_DEFAULT_COMPRESSION
    Level1 = 1,
    Level2 = 2,
    Level3 = 3,
    Level4 = 4,
    Level5 = 5,
    Level6 = 6,
    Level7 = 7,
    Level8 = 8,
    Level9 = 9,
    BestSpeed = Level1, //!<  -> Z_BEST_SPEED
    BestSize = Level9   //!<  -> Z_BEST_COMPRESSION
};

//! zlib compression strategy
enum class ZLIB_CompressStrategy {
    Default = 0,     //!<  -> Z_DEFAULT_STRATEGY
    Filtered = 1,    //!<  -> Z_FILTERED
    HuffmanOnly = 2, //!<  -> Z_HUFFMAN_ONLY
    RLE = 3,         //!<  -> Z_RLE
    Fixed = 4        //!<  -> Z_FIXED
};

//! zlib compression options
//!
//! Initialising ZLIB_CompressOptions with \c {} in C++ is the convenient way to
//! set default values.
struct ZLIB_CompressOptions {
    //! Compression level
    ZLIB_CompressLevel level;

    //! Compression strategy
    ZLIB_CompressStrategy strategy;

    //! Specifies how much memory should be allocated for the internal
    //! compression state
    //!
    //! The value must belongs to \c [1..9] or equals to \c 0 which maps to the
    //! default usage.
    //!
    //! \c 1 uses minimum memory but is slow and reduces compression ratio\n
    //! \c 9 uses maximum memory for optimal speed
    uint8_t memory_usage;

    //! Optional function used to allocate the internal state within \c z_stream
    //! \sa z_stream::zalloc
    void* (*func_alloc)(void* opaque, unsigned int items, unsigned int size);

    //! Optional function used to free the internal state within \c z_stream
    //! \sa z_stream::zfree
    void  (*func_free)(void* opaque, void* address);

    //! Optional private data object passed to func_alloc() and func_free()
    //! \sa z_stream::opaque
    void* opaque;
};

} // namespace gmio

/*! @} */
