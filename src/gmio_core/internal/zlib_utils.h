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

#pragma once

#include "../global.h"
#include "../zlib_compress.h"
#include <zlib.h>

/*! Converts zlib error to gmio "zlib-specific" error */
int zlib_error_to_gmio_error(int z_error);

/*! Wrapper around deflateInit2(), returns a converted gmio error code */
int gmio_zlib_compress_init(
        struct z_stream_s* z_stream,
        const struct gmio_zlib_compress_options* z_opts);

/*! Checks zlib compression options */
bool gmio_check_zlib_compress_options(
        int* error, const struct gmio_zlib_compress_options* z_opts);

/*! Decompresses the source buffer into the destination buffer.
 *  \p src_len is the byte length of the source buffer. Upon entry, \p dest_len
 *  is the total size of the destination buffer, which must be large enough to
 *  hold the entire uncompressed data.
 *  Upon exit, \p dest_len is the actual size of the compressed buffer. */
int gmio_zlib_uncompress_buffer(
        uint8_t* dest, size_t* dest_len, const uint8_t* src, size_t src_len);

/*! Computes the CRC-32 value with the bytes from \p buff */
uint32_t gmio_zlib_crc32(const uint8_t* buff, size_t buff_len);

/*! Returns the required initial value for gmio_zlib_crc32_update() */
uint32_t gmio_zlib_crc32_initial();

/*! Updates a running CRC-32 with the bytes from \p buff */
uint32_t gmio_zlib_crc32_update(
        uint32_t crc, const uint8_t* buff, size_t buff_len);
