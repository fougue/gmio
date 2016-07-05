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

#ifndef GMIO_INTERNAL_BYTE_CODEC_H
#define GMIO_INTERNAL_BYTE_CODEC_H

#include "../global.h"

/*! Reads a 16bit integer from memory-location \p bytes (little-endian) */
GMIO_INLINE uint16_t gmio_decode_uint16_le(const uint8_t* bytes)
{
    /* |0 |1 | */
    /* |BB|AA| -> 0xAABB */
    return (bytes[1] << 8) | bytes[0];
}

/*! Reads a 16bit integer from memory-location \p bytes (big-endian) */
GMIO_INLINE uint16_t gmio_decode_uint16_be(const uint8_t* bytes)
{
    /* |0 |1 | */
    /* |AA|BB| -> 0xAABB */
    return (bytes[0] << 8) | bytes[1];
}

/*! Writes 16bit integer \p val to the memory location at \p bytes in
 *  little-endian */
GMIO_INLINE void gmio_encode_uint16_le(uint16_t val, uint8_t* bytes)
{
    bytes[0] = val & 0xFF;
    bytes[1] = (val >> 8) & 0xFF;
}

/*! Reads a 32bit integer from memory-location \p bytes (little-endian) */
GMIO_INLINE uint32_t gmio_decode_uint32_le(const uint8_t* bytes)
{
    /* |DD|CC|BB|AA| -> 0xAABBCCDD */
    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

/*! Reads a 32bit integer from memory-location \p bytes (mixed-endian) */
GMIO_INLINE uint32_t gmio_decode_uint32_me(const uint8_t* bytes)
{
    /* |DD|CC|BB|AA| -> 0xCCDDAABB */
    return (bytes[0] << 16) | (bytes[1] << 24) | (bytes[3] << 8) | bytes[2];
}

/*! Reads a 32bit integer from memory-location \p bytes (big-endian) */
GMIO_INLINE uint32_t gmio_decode_uint32_be(const uint8_t* bytes)
{
    /* |DD|CC|BB|AA| -> 0xDDCCBBAA */
    return bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24);
}

/*! Writes 32bit integer \p val to the memory location at \p bytes in
 *  little-endian */
GMIO_INLINE void gmio_encode_uint32_le(uint32_t val, uint8_t* bytes)
{
    bytes[0] = val & 0xFF;
    bytes[1] = (val >> 8) & 0xFF;
    bytes[2] = (val >> 16) & 0xFF;
    bytes[3] = (val >> 24) & 0xFF;
}

/*! Writes 32bit integer \p val to the memory location at \p bytes in
 *  big-endian */
GMIO_INLINE void gmio_encode_uint32_be(uint32_t val, uint8_t* bytes)
{
    bytes[0] = (val >> 24) & 0xFF;
    bytes[1] = (val >> 16) & 0xFF;
    bytes[2] = (val >> 8) & 0xFF;
    bytes[3] = val & 0xFF;
}

#endif /* GMIO_INTERNAL_BYTE_CODEC_H */
