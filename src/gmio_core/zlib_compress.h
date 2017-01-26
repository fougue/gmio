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

#ifndef GMIO_ZLIB_COMPRESS_H
#define GMIO_ZLIB_COMPRESS_H

#include "global.h"

/*! zlib compression level, specific values */
enum gmio_zlib_compress_level
{
    GMIO_ZLIB_COMPRESS_LEVEL_NONE = -1,      /*! -> Z_NO_COMPRESSION */
    GMIO_ZLIB_COMPRESS_LEVEL_DEFAULT = 0,    /*! -> Z_DEFAULT_COMPRESSION */
    GMIO_ZLIB_COMPRESS_LEVEL_BEST_SPEED = 1, /*! -> Z_BEST_SPEED */
    GMIO_ZLIB_COMPRESS_LEVEL_BEST_SIZE = 9   /*! -> Z_BEST_COMPRESSION */
};

/*! zlib compression strategy */
enum gmio_zlib_compress_strategy
{
    GMIO_ZLIB_COMPRESSION_STRATEGY_DEFAULT = 0,   /*! -> Z_DEFAULT_STRATEGY */
    GMIO_ZLIB_COMPRESSION_STRATEGY_FILTERED = 1,  /*! -> Z_FILTERED */
    GMIO_ZLIB_COMPRESSION_STRATEGY_HUFFMAN_ONLY = 2, /*! -> Z_HUFFMAN_ONLY */
    GMIO_ZLIB_COMPRESSION_STRATEGY_RLE = 3,       /*! -> Z_RLE */
    GMIO_ZLIB_COMPRESSION_STRATEGY_FIXED = 4      /*! -> Z_FIXED */
};

/*! zlib compression options
 *
 *  Initialising gmio_zlib_compress_options with \c {0} (or \c {} in C++) is the
 *  convenient way to set default values.
*/
struct gmio_zlib_compress_options
{
    /*! Compression level.
     *  Use enum value from \c gmio_zlib_compress_level */
    uint8_t level;

    /*! Compression strategy */
    enum gmio_zlib_compress_strategy strategy;

    /*! Specifies how much memory should be allocated for the internal
     *  compression state
     *
     *  The value must belongs to \c [1..9] or equals to \c 0 which maps to the
     *  default usage.
     *
     *  \c 1 uses minimum memory but is slow and reduces compression ratio
     *  \c 9 uses maximum memory for optimal speed */
    uint8_t memory_usage;

    /*! Optional pointer to a function used to allocate the internal state
     *  within \c z_stream structure.
     *  \sa z_stream::zalloc */
    void* (*func_alloc)(void* opaque, unsigned int items, unsigned int size);

    /*! Optional pointer to a function used to free the internal state within
     *  \c z_stream structure.
     *  \sa z_stream::zfree */
    void  (*func_free)(void* opaque, void* address);

    /*! Optional private data object passed to func_alloc and func_free.
     *  \sa z_stream::opaque */
    void* opaque;
};

#endif /* GMIO_ZLIB_COMPRESS_H */
/*! @} */
