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

/*! \file amf_io_options.h
 *  Options for AMF read/write functions
 *
 *  \addtogroup gmio_amf
 *  @{
 */

#ifndef GMIO_AMF_IO_OPTIONS_H
#define GMIO_AMF_IO_OPTIONS_H

#include "amf_global.h"
#include "../gmio_core/memblock.h"
#include "../gmio_core/task_iface.h"
#include "../gmio_core/text_format.h"

/*! zlib compression level, specific values */
enum gmio_zlib_compress_level
{
    GMIO_ZLIB_COMPRESS_LEVEL_NONE = -1,      /*! Maps to Z_NO_COMPRESSION */
    GMIO_ZLIB_COMPRESS_LEVEL_DEFAULT = 0,    /*! Maps to Z_DEFAULT_COMPRESSION */
    GMIO_ZLIB_COMPRESS_LEVEL_BEST_SPEED = 1, /*! Maps to Z_BEST_SPEED */
    GMIO_ZLIB_COMPRESS_LEVEL_BEST_SIZE = 9   /*! Maps to Z_BEST_COMPRESSION */
};

/*! zlib compression strategy */
enum gmio_zlib_compress_strategy
{
    GMIO_ZLIB_COMPRESSION_STRATEGY_DEFAULT = 0,
    GMIO_ZLIB_COMPRESSION_STRATEGY_FILTERED = 1,
    GMIO_ZLIB_COMPRESSION_STRATEGY_HUFFMAN_ONLY = 2,
    GMIO_ZLIB_COMPRESSION_STRATEGY_RLE = 3,
    GMIO_ZLIB_COMPRESSION_STRATEGY_FIXED = 4
};

/*! zlib compression options */
struct gmio_zlib_compress_options
{
    /*! Compression level
     *
     *  \c 0 : default compression
     *  \c 1 : best speed
     *  \c 9 : best compression */
    uint8_t level;

    /*! Compression strategy
     *
     *  \c 0 : default strategy
     *  \c 1 : filtered
     *  \c 9 : best compression */
    enum gmio_zlib_compress_strategy strategy;

    /*! Specifies how much memory should be allocated for the internal
     *  compression state
     *
     *  The value must belongs to \c [1..9] or equals to \c 0 which maps to the
     *  default usage.
     *
     *  \c 1 uses minimum memory but is slow and reduces compression ratio
     *  \c 9 uses maximum memory for optimal speed
     */
    uint8_t memory_usage;
};

struct gmio_amf_write_options
{
    /*! Used by the stream to bufferize I/O operations
     *
     *  If null, then a temporary memblock is created with the global default
     *  constructor function
     *
     *  \sa gmio_memblock_isnull()
     *  \sa gmio_memblock_default() */
    struct gmio_memblock stream_memblock;

    /*! Optional interface by which the I/O operation can be controlled */
    struct gmio_task_iface task_iface;

    /*! The format used when writting double values as strings
     *
     *  Defaulted to \c GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE when calling
     *  gmio_amf_write() with \c options==NULL
     */
    enum gmio_float_text_format float64_format;

    /*! The maximum number of significant digits when writting double values
     *
     *  Defaulted to \c 16 when calling gmio_amf_write() with \c options==NULL
     */
    uint8_t float64_prec;

    /* ZIP compression */

    bool compress;
    struct gmio_zlib_compress_options z_compress_options;
};

#endif /* GMIO_AMF_IO_OPTIONS_H */
