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

namespace gmio {

// TODO Rename :
//      adv_decode_uintNN_le() -> uintNN_decodeAdvLittleEndian()
//      adv_decode_uintNN_be() -> uintNN_decodeAdvBigEndian()
//      adv_encode_uintNN_le() -> uintNN_encodeAdvLittleEndian()
//      adv_encode_uintNN_be() -> uintNN_encodeAdvBigEndian()

/* Functions that read NNbit uint from memory (little-endian)
 *   uintNN_t decode_uintNN_le(const uint8_t* bytes)
 *
 * Functions that read NNbit uint from memory (big-endian)
 *   uintNN_t decode_uintNN_be(const uint8_t* bytes)
 *
 * Functions that writes NNbit uint to memory in little-endian
 *   void encode_uintNN_le(uintNN_t val, uint8_t* bytes);
 *
 * Functions that writes NNbit uint to memory in big-endian
 *   void encode_uintNN_be(uintNN_t val, uint8_t* bytes);
 */

constexpr uint16_t uint16_decodeLittleEndian(const uint8_t* bytes);
constexpr uint32_t uint32_decodeLittleEndian(const uint8_t* bytes);
inline    uint64_t uint64_decodeLittleEndian(const uint8_t* bytes);
constexpr uint16_t uint16_decodeBigEndian(const uint8_t* bytes);
constexpr uint32_t uint32_decodeBigEndian(const uint8_t* bytes);
inline    uint64_t uint64_decodeBigEndian(const uint8_t* bytes);
inline void uint16_encodeLittleEndian(uint16_t val, uint8_t* bytes);
inline void uint32_encodeLittleEndian(uint32_t val, uint8_t* bytes);
inline void uint64_encodeLittleEndian(uint64_t val, uint8_t* bytes);
inline void uint16_encodeBigEndian(uint16_t val, uint8_t* bytes);
inline void uint32_encodeBigEndian(uint32_t val, uint8_t* bytes);
inline void uint64_encodeBigEndian(uint64_t val, uint8_t* bytes);

/* Functions that reads NNbit uint from memory (little-endian) and advances
 * buffer pos
 *   uintNN_t adv_decode_uintNN_le(const uint8_t** bytes)
 *
 * Functions that reads NNbit uint from memory (big-endian) and advances
 * buffer pos
 *   uintNN_t adv_decode_uintNN_be(const uint8_t** bytes)
 *
 * Functions that writes NNbit uint to memory in little-endian and returns
 * advanced buffer pos
 *   uint8_t* adv_encode_uintNN_le(uintNN_t val, uint8_t* bytes)
 *
 * Functions that writes NNbit uint to memory in big-endian and returns
 * advanced buffer pos
 *   uint8_t* adv_encode_uintNN_be(uintNN_t val, uint8_t* bytes)
 */

inline uint16_t adv_decode_uint16_le(const uint8_t** bytes);
inline uint16_t adv_decode_uint16_be(const uint8_t** bytes);
inline uint32_t adv_decode_uint32_le(const uint8_t** bytes);
inline uint32_t adv_decode_uint32_be(const uint8_t** bytes);
inline void adv_encode_uint16_le(uint16_t val, uint8_t** bytes);
inline void adv_encode_uint16_be(uint16_t val, uint8_t** bytes);
inline void adv_encode_uint32_le(uint32_t val, uint8_t** bytes);
inline void adv_encode_uint32_be(uint32_t val, uint8_t** bytes);

inline uint64_t adv_decode_uint64_le(const uint8_t** bytes);
inline uint64_t adv_decode_uint64_be(const uint8_t** bytes);
inline void adv_encode_uint64_le(uint64_t val, uint8_t** bytes);
inline void adv_encode_uint64_be(uint64_t val, uint8_t** bytes);



/*
 * Implementation
 */

constexpr uint16_t uint16_decodeLittleEndian(const uint8_t* bytes)
{
    // |0 |1 |
    // |BB|AA| -> 0xAABB
    return (bytes[1] << 8) | bytes[0];
}

constexpr uint32_t uint32_decodeLittleEndian(const uint8_t* bytes)
{
    // |DD|CC|BB|AA| -> 0xAABBCCDD
    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

uint64_t uint64_decodeLittleEndian(const uint8_t* bytes)
{
    const uint64_t h32 = uint32_decodeLittleEndian(bytes + 4); // Decode high 32b
    return (h32 << 32) | uint32_decodeLittleEndian(bytes);
}

constexpr uint16_t uint16_decodeBigEndian(const uint8_t* bytes)
{
    // |0 |1 |
    // |AA|BB| -> 0xAABB
    return (bytes[0] << 8) | bytes[1];
}

constexpr uint32_t uint32_decodeBigEndian(const uint8_t* bytes)
{
    // |DD|CC|BB|AA| -> 0xDDCCBBAA
    return bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24);
}

uint64_t uint64_decodeBigEndian(const uint8_t* bytes)
{
    const uint64_t l32 = uint32_decodeBigEndian(bytes); // Decode low 32b
    return (l32 << 32) | uint32_decodeBigEndian(bytes + 4);
}

void uint16_encodeLittleEndian(uint16_t val, uint8_t* bytes)
{
    bytes[0] = val & 0xFF;
    bytes[1] = (val >> 8) & 0xFF;
}

void uint32_encodeLittleEndian(uint32_t val, uint8_t* bytes)
{
    bytes[0] = val & 0xFF;
    bytes[1] = (val >> 8) & 0xFF;
    bytes[2] = (val >> 16) & 0xFF;
    bytes[3] = (val >> 24) & 0xFF;
}

void uint64_encodeLittleEndian(uint64_t val, uint8_t* bytes)
{
    bytes[0] = val & 0xFF;
    bytes[1] = (val >> 8)  & 0xFF;
    bytes[2] = (val >> 16) & 0xFF;
    bytes[3] = (val >> 24) & 0xFF;
    bytes[4] = (val >> 32) & 0xFF;
    bytes[5] = (val >> 40) & 0xFF;
    bytes[6] = (val >> 48) & 0xFF;
    bytes[7] = (val >> 56) & 0xFF;
}

void uint16_encodeBigEndian(uint16_t val, uint8_t* bytes)
{
    bytes[0] = (val >> 8) & 0xFF;
    bytes[1] = val & 0xFF;
}

void uint32_encodeBigEndian(uint32_t val, uint8_t* bytes)
{
    bytes[0] = (val >> 24) & 0xFF;
    bytes[1] = (val >> 16) & 0xFF;
    bytes[2] = (val >> 8) & 0xFF;
    bytes[3] = val & 0xFF;
}

void uint64_encodeBigEndian(uint64_t val, uint8_t* bytes)
{
    bytes[0] = (val >> 56) & 0xFF;
    bytes[1] = (val >> 48) & 0xFF;
    bytes[2] = (val >> 40) & 0xFF;
    bytes[3] = (val >> 32) & 0xFF;
    bytes[4] = (val >> 24) & 0xFF;
    bytes[5] = (val >> 16) & 0xFF;
    bytes[6] = (val >> 8)  & 0xFF;
    bytes[7] = val & 0xFF;
}

uint16_t adv_decode_uint16_le(const uint8_t** bytes)
{
    const uint16_t val = uint16_decodeLittleEndian(*bytes);
    *bytes += 2;
    return val;
}

uint16_t adv_decode_uint16_be(const uint8_t** bytes)
{
    const uint16_t val = uint16_decodeBigEndian(*bytes);
    *bytes += 2;
    return val;
}

uint32_t adv_decode_uint32_le(const uint8_t** bytes)
{
    const uint32_t val = uint32_decodeLittleEndian(*bytes);
    *bytes += 4;
    return val;
}

uint32_t adv_decode_uint32_be(const uint8_t** bytes)
{
    const uint32_t val = uint32_decodeBigEndian(*bytes);
    *bytes += 4;
    return val;
}

void adv_encode_uint16_le(uint16_t val, uint8_t** bytes)
{
    uint16_encodeLittleEndian(val, *bytes);
    *bytes += 2;
}

void adv_encode_uint16_be(uint16_t val, uint8_t** bytes)
{
    uint16_encodeBigEndian(val, *bytes);
    *bytes += 2;
}

void adv_encode_uint32_le(uint32_t val, uint8_t** bytes)
{
    uint32_encodeLittleEndian(val, *bytes);
    *bytes += 4;
}

void adv_encode_uint32_be(uint32_t val, uint8_t** bytes)
{
    uint32_encodeBigEndian(val, *bytes);
    *bytes += 4;
}

uint64_t adv_decode_uint64_le(const uint8_t** bytes)
{
    const uint64_t val = uint64_decodeLittleEndian(*bytes);
    *bytes += 8;
    return val;
}

uint64_t adv_decode_uint64_be(const uint8_t** bytes)
{
    const uint64_t val = uint64_decodeBigEndian(*bytes);
    *bytes += 8;
    return val;
}

void adv_encode_uint64_le(uint64_t val, uint8_t** bytes)
{
    uint64_encodeLittleEndian(val, *bytes);
    *bytes += 8;
}

void adv_encode_uint64_be(uint64_t val, uint8_t** bytes)
{
    uint64_encodeBigEndian(val, *bytes);
    *bytes += 8;
}

} // namespace gmio
