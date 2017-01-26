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

#include "zlib_utils.h"
#include "../error.h"

/* zlib doc:
 *   the windowBits parameter is the base two logarithm of the window size
 *   (the size of the history buffer). It should be in the range 8..15 for
 *   this version of the library. Larger values of this parameter result in
 *   better compression at the expense of memory usage. The default value is
 *   15 if deflateInit is used instead.
 *   windowBits can also be –8..–15 for raw deflate. In this case, -windowBits
 *   determines the window size. deflate() will then generate raw deflate
 *   data with no zlib header or trailer, and will not compute an adler32
 *   check value. */
static const int z_window_bits_for_no_zlib_wrapper = -15;

static int gmio_to_zlib_compress_level(int gmio_compress_level)
{
    if (gmio_compress_level == GMIO_ZLIB_COMPRESS_LEVEL_DEFAULT)
        return Z_DEFAULT_COMPRESSION;
    else if (gmio_compress_level == GMIO_ZLIB_COMPRESS_LEVEL_NONE)
        return Z_NO_COMPRESSION;
    return gmio_compress_level;
}

static int gmio_to_zlib_compress_memusage(int gmio_compress_memusage)
{
    if (gmio_compress_memusage == 0)
        return 8;
    return gmio_compress_memusage;
}

/* Converts zlib error to gmio "zlib-specific" error */
int zlib_error_to_gmio_error(int z_error)
{
    switch (z_error) {
    case Z_OK: return GMIO_ERROR_OK;
    case Z_ERRNO: return GMIO_ERROR_ZLIB_ERRNO;
    case Z_STREAM_ERROR: return GMIO_ERROR_ZLIB_STREAM;
    case Z_DATA_ERROR: return GMIO_ERROR_ZLIB_DATA;
    case Z_MEM_ERROR: return GMIO_ERROR_ZLIB_MEM;
    case Z_BUF_ERROR: return GMIO_ERROR_ZLIB_BUF;
    case Z_VERSION_ERROR: return GMIO_ERROR_ZLIB_VERSION;
    }
    return GMIO_ERROR_UNKNOWN;
}

int gmio_zlib_compress_init(
        struct z_stream_s* z_stream,
        const struct gmio_zlib_compress_options* z_opts)
{
    const int zlib_compress_level =
            gmio_to_zlib_compress_level(z_opts->level);
    const int zlib_compress_memusage =
            gmio_to_zlib_compress_memusage(z_opts->memory_usage);
    const int z_init_error =
            deflateInit2_(
                z_stream,
                zlib_compress_level,
                Z_DEFLATED, /* Method */
                z_window_bits_for_no_zlib_wrapper,
                zlib_compress_memusage,
                z_opts->strategy,
                ZLIB_VERSION,
                sizeof(struct z_stream_s));
    return zlib_error_to_gmio_error(z_init_error);
}

bool gmio_check_zlib_compress_options(
        int* error, const struct gmio_zlib_compress_options* z_opts)
{
    if (z_opts != NULL) {
        if (z_opts->level > 9)
            *error = GMIO_ERROR_ZLIB_INVALID_COMPRESS_LEVEL;
        if (z_opts->memory_usage > 9)
            *error = GMIO_ERROR_ZLIB_INVALID_COMPRESS_MEMORY_USAGE;
    }
    return gmio_no_error(*error);
}

int gmio_zlib_uncompress_buffer(
        uint8_t* dest, size_t* dest_len, const uint8_t* src, size_t src_len)
{
    z_stream stream;
    int err;

    stream.next_in = (z_const Bytef *)src;
    stream.avail_in = (uInt)src_len;
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != src_len)
        return GMIO_ERROR_ZLIB_BUF;

    stream.next_out = dest;
    stream.avail_out = (uInt)*dest_len;
    if ((uLong)stream.avail_out != *dest_len)
        return GMIO_ERROR_ZLIB_BUF;

    stream.zalloc = (alloc_func)NULL;
    stream.zfree = (free_func)NULL;

    err = inflateInit2_(
                &stream,
                z_window_bits_for_no_zlib_wrapper,
                ZLIB_VERSION,
                sizeof(z_stream));
    if (err != Z_OK)
        return err;

    err = inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
            return GMIO_ERROR_ZLIB_DATA;
        return err;
    }
    *dest_len = stream.total_out;

    err = inflateEnd(&stream);
    return zlib_error_to_gmio_error(err);
}

uint32_t gmio_zlib_crc32(const uint8_t *buff, size_t buff_len)
{
    return gmio_zlib_crc32_update(gmio_zlib_crc32_initial(), buff, buff_len);
}

uint32_t gmio_zlib_crc32_update(
        uint32_t crc, const uint8_t *buff, size_t buff_len)
{
    return crc32(crc, (const Bytef*)buff, (uInt)buff_len);
}

uint32_t gmio_zlib_crc32_initial()
{
    return crc32(0, NULL, 0);
}
