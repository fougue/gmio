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

namespace gmio {

namespace {

// zlib doc:
//   the windowBits parameter is the base two logarithm of the window size
//   (the size of the history buffer). It should be in the range 8..15 for
//   this version of the library. Larger values of this parameter result in
//   better compression at the expense of memory usage. The default value is
//   15 if deflateInit is used instead.
//   windowBits can also be –8..–15 for raw deflate. In this case, -windowBits
//   determines the window size. deflate() will then generate raw deflate
//   data with no zlib header or trailer, and will not compute an adler32
//   check value.
const int z_window_bits_for_no_zlib_wrapper = -15;

int ZLIB_toCompressLevel(ZLIB_CompressLevel gmio_compress_level)
{
    if (gmio_compress_level == ZLIB_CompressLevel::Default)
        return Z_DEFAULT_COMPRESSION;
    else if (gmio_compress_level == ZLIB_CompressLevel::None)
        return Z_NO_COMPRESSION;
    return static_cast<int>(gmio_compress_level);
}

int ZLIB_toCompressMemUsage(int gmio_compress_memusage)
{
    if (gmio_compress_memusage == 0)
        return 8;
    return gmio_compress_memusage;
}

} // namespace

// Converts zlib error to gmio "zlib-specific" error
int ZLIB_toGmioError(int z_error)
{
    switch (z_error) {
    case Z_OK: return Error_OK;
    case Z_ERRNO: return Error_Zlib_Errno;
    case Z_STREAM_ERROR: return Error_Zlib_Stream;
    case Z_DATA_ERROR: return Error_Zlib_Data;
    case Z_MEM_ERROR: return Error_Zlib_Mem;
    case Z_BUF_ERROR: return Error_ZLib_Buf;
    case Z_VERSION_ERROR: return Error_Zlib_Version;
    }
    return Error_Unknown;
}

int ZLIB_initCompress(
        z_stream_s* z_stream, const ZLIB_CompressOptions& z_opts)
{
    const int zlib_compress_level = ZLIB_toCompressLevel(z_opts.level);
    const int zlib_compress_memusage = ZLIB_toCompressMemUsage(z_opts.memory_usage);
    const int z_init_error =
            deflateInit2_(
                z_stream,
                zlib_compress_level,
                Z_DEFLATED, // Method
                z_window_bits_for_no_zlib_wrapper,
                zlib_compress_memusage,
                static_cast<int>(z_opts.strategy),
                ZLIB_VERSION,
                sizeof(struct z_stream_s));
    return ZLIB_toGmioError(z_init_error);
}

bool ZLIB_checkCompressOptions(
        int* error, const ZLIB_CompressOptions& z_opts)
{
    if (z_opts.memory_usage > 9)
        *error = Error_Zlib_InvalidCompressMemoryUsage;
    return *error == Error_OK;
}

int ZLIB_uncompressBuffer(
        uint8_t* dest, size_t* dest_len, const uint8_t* src, size_t src_len)
{
    z_stream stream;
    int err;

    ZLIB_assignNextIn(&stream, src, src_len);
    // Check for source > 64K on 16-bit machine:
    if (static_cast<uLong>(stream.avail_in) != src_len)
        return Error_ZLib_Buf;

    ZLIB_assignNextOut(&stream, dest, *dest_len);
    if (static_cast<uLong>(stream.avail_out) != *dest_len)
        return Error_ZLib_Buf;

    stream.zalloc = nullptr;
    stream.zfree = nullptr;

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
            return Error_Zlib_Data;
        return err;
    }
    *dest_len = stream.total_out;

    err = inflateEnd(&stream);
    return ZLIB_toGmioError(err);
}

uint32_t ZLIB_crc32(const uint8_t *buff, size_t buff_len)
{
    return ZLIB_crc32Update(ZLIB_crc32Initial(), buff, buff_len);
}

uint32_t ZLIB_crc32Update(uint32_t crc, const uint8_t *buff, size_t buff_len)
{
    return crc32(crc, static_cast<const Bytef*>(buff), static_cast<uInt>(buff_len));
}

uint32_t ZLIB_crc32Initial()
{
    return crc32(0, nullptr, 0);
}

void ZLIB_assignNextIn(
        z_stream_s *zstream, const uint8_t *next_in, size_t avail_in)
{
    zstream->next_in = const_cast<z_const Bytef*>(next_in);
    zstream->avail_in = static_cast<uInt>(avail_in);
}

void ZLIB_assignNextOut(
        z_stream_s *zstream, uint8_t *next_out, size_t avail_out)
{
    zstream->next_out = next_out;
    zstream->avail_out = static_cast<uInt>(avail_out);
}

} // namespace gmio
