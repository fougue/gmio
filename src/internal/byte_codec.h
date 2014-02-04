/* Warning : this header has no multi-inclusion guard. It must be included only once in the
 *           translation unit of use. The reason is that all functions defined here are meant to
 *           be inlined for performance purpose
 */

#include "../global.h"

/*! Read a 16bit integer from memory-location \p bytes (little-endian byte order) */
FOUG_INLINE static uint16_t foug_decode_uint16_le(const uint8_t* bytes)
{
  /* |0 |1 | */
  /* |BB|AA| -> 0xAABB */
  return (bytes[1] << 8) | bytes[0];
}

/*! Read a 16bit integer from memory-location \p bytes (big-endian byte order) */
FOUG_INLINE static uint16_t foug_decode_uint16_be(const uint8_t* bytes)
{
  /* |0 |1 | */
  /* |AA|BB| -> 0xAABB */
  return (bytes[0] << 8) | bytes[1];
}

/*! Write 16bit integer \p val to the memory location at \p bytes in little-endian byte order */
FOUG_INLINE static void foug_encode_uint16_le(uint16_t val, uint8_t* bytes)
{
  bytes[0] = val & 0xFF;
  bytes[1] = (val >> 8) & 0xFF;
}

/*! Read a 32bit integer from memory-location \p bytes (little-endian byte order) */
FOUG_INLINE static uint32_t foug_decode_uint32_le(const uint8_t* bytes)
{
  /* |DD|CC|BB|AA| -> 0xAABBCCDD */
  return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

/*! Read a 32bit integer from memory-location \p bytes (mixed-endian byte order) */
FOUG_INLINE static uint32_t foug_decode_uint32_me(const uint8_t* bytes)
{
  /* |DD|CC|BB|AA| -> 0xCCDDAABB */
  return (bytes[0] << 16) | (bytes[1] << 24) | (bytes[3] << 8) | bytes[2];
}

/*! Read a 32bit integer from memory-location \p bytes (big-endian byte order) */
FOUG_INLINE static uint32_t foug_decode_uint32_be(const uint8_t* bytes)
{
  /* |DD|CC|BB|AA| -> 0xDDCCBBAA */
  return bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24);
}

/*! Write 32bit integer \p val to the memory location at \p bytes in little-endian byte order */
FOUG_INLINE static void foug_encode_uint32_le(uint32_t val, uint8_t* bytes)
{
  bytes[0] = val & 0xFF;
  bytes[1] = (val >> 8) & 0xFF;
  bytes[2] = (val >> 16) & 0xFF;
  bytes[3] = (val >> 24) & 0xFF;
}

/*! Write 32bit integer \p val to the memory location at \p bytes in big-endian byte order */
FOUG_INLINE static void foug_encode_uint32_be(uint32_t val, uint8_t* bytes)
{
  bytes[0] = (val >> 24) & 0xFF;
  bytes[1] = (val >> 16) & 0xFF;
  bytes[2] = (val >> 8) & 0xFF;
  bytes[3] = val & 0xFF;
}
