/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
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
