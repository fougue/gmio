#include "endian.h"

#include "convert.h"

#ifdef FOUG_HAVE_MSVC_BUILTIN_BSWAP_FUNC
# include <stdlib.h>
#endif
#include <string.h>

typedef union
{
  uint32_t integer;
  uint8_t bytes[4];
} _internal_foug_int_bytes_32_convert_t;

/*! Endianness (byte order) of the host's CPU architecture */
foug_endianness_t foug_host_endianness()
{
  _internal_foug_int_bytes_32_convert_t conv;

  conv.integer = 0x01020408;
  if (conv.bytes[0] == 0x08 && conv.bytes[3] == 0x01)
    return FOUG_LITTLE_ENDIAN;
  else if (conv.bytes[0] == 0x01 && conv.bytes[3] == 0x08)
    return FOUG_BIG_ENDIAN;
  else if (conv.bytes[1] == 0x08 && conv.bytes[2] == 0x01)
    return FOUG_MIDDLE_ENDIAN;
  else
    return FOUG_OTHER_ENDIAN;
}

/*! Returns \p val with the order of bytes reversed.
 *
 *  Uses compiler builtin functions if available
 */
uint16_t foug_uint16_bswap(uint16_t val)
{
#ifdef FOUG_HAVE_GCC_BUILTIN_BSWAP_FUNC
  return __builtin_bswap16(val);
#elif defined(FOUG_HAVE_MSVC_BUILTIN_BSWAP_FUNC)
  return _byteswap_ushort(val);
#else
  return ((val & 0x00FF) << 8) | ((val >> 8) & 0x00FF);
#endif
}

/*! Read a 16bit integer from memory-location \p bytes (little-endian byte order) */
uint16_t foug_decode_uint16_le(const uint8_t* bytes)
{
  /* |0 |1 | */
  /* |BB|AA| -> 0xAABB */
  return (bytes[1] << 8) | bytes[0];
}

/*! Read a 16bit integer from memory-location \p bytes (big-endian byte order) */
uint16_t foug_decode_uint16_be(const uint8_t* bytes)
{
  /* |0 |1 | */
  /* |AA|BB| -> 0xAABB */
  return (bytes[0] << 8) | bytes[1];
}

/*! Write 16bit integer \p val to the memory location at \p bytes in little-endian byte order */
void foug_encode_uint16_le(uint16_t val, uint8_t* bytes)
{
  bytes[0] = val & 0xFF;
  bytes[1] = (val >> 8) & 0xFF;
}

/*! Returns \p val with the order of bytes reversed.
 *
 *  Uses compiler builtin functions if available
 */
uint32_t foug_uint32_bswap(uint32_t val)
{
#ifdef FOUG_HAVE_GCC_BUILTIN_BSWAP_FUNC
  return __builtin_bswap32(val);
#elif defined(FOUG_HAVE_MSVC_BUILTIN_BSWAP_FUNC)
  return _byteswap_ulong(val);
#else
  return
      ((val & 0x000000FF) << 24)
      | ((val & 0x0000FF00) << 8)
      | ((val >> 8) & 0x0000FF00)
      | ((val >> 24) & 0x000000FF);
#endif
}

/*! Read a 32bit integer from memory-location \p bytes (little-endian byte order) */
uint32_t foug_decode_uint32_le(const uint8_t* bytes)
{
  /* |DD|CC|BB|AA| -> 0xAABBCCDD */
  return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

/*! Read a 32bit integer from memory-location \p bytes (mixed-endian byte order) */
uint32_t foug_decode_uint32_me(const uint8_t* bytes)
{
  /* |DD|CC|BB|AA| -> 0xCCDDAABB */
  return (bytes[0] >> 8) | (bytes[2] << 8) | (bytes[3] << 8) | (bytes[1] << 8);
}

/*! Read a 32bit integer from memory-location \p bytes (big-endian byte order) */
uint32_t foug_decode_uint32_be(const uint8_t* bytes)
{
  /* |DD|CC|BB|AA| -> 0xDDCCBBAA */
  return bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24);
}

/*! Write 32bit integer \p val to the memory location at \p bytes in little-endian byte order */
void foug_encode_uint32_le(uint32_t val, uint8_t* bytes)
{
  bytes[0] = val & 0xFF;
  bytes[1] = (val >> 8) & 0xFF;
  bytes[2] = (val >> 16) & 0xFF;
  bytes[3] = (val >> 24) & 0xFF;
}

/*! Write 32bit integer \p val to the memory location at \p bytes in big-endian byte order */
void foug_encode_uint32_be(uint32_t val, uint8_t* bytes)
{
  bytes[0] = (val >> 24) & 0xFF;
  bytes[1] = (val >> 16) & 0xFF;
  bytes[2] = (val >> 8) & 0xFF;
  bytes[3] = val & 0xFF;
}

/*! Read a 32bit real from memory-location \p bytes (little-endian byte order) */
foug_real32_t foug_decode_real32_le(const uint8_t* bytes)
{
  return foug_convert_real32(foug_decode_uint32_le(bytes));
}

/*! Read a 32bit real from memory-location \p bytes (mixed-endian byte order) */
foug_real32_t foug_decode_real32_me(const uint8_t* bytes)
{
  return foug_convert_real32(foug_decode_uint32_me(bytes));
}

/*! Read a 32bit real from memory-location \p bytes (big-endian byte order) */
foug_real32_t foug_decode_real32_be(const uint8_t* bytes)
{
  return foug_convert_real32(foug_decode_uint32_be(bytes));
}

/*! Write 32bit real \p val to the memory location at \p bytes in little-endian byte order */
void foug_encode_real32_le(foug_real32_t val, uint8_t* bytes)
{
  foug_encode_uint32_le(foug_convert_uint32(val), bytes);
}
