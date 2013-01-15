#include "endian.h"

#include <string.h>

#define _INTERNAL_FOUG_ENCODE_NUMERIC_LE(NUMERIC_TYPE) \
  union { \
    NUMERIC_TYPE as_numeric; \
    uint8_t as_bytes[sizeof(NUMERIC_TYPE)]; \
  } u; \
  u.as_numeric = val; \
  memcpy(bytes, u.as_bytes, sizeof(NUMERIC_TYPE));

foug_endianness_t foug_host_endianness()
{
  union {
    uint32_t integer;
    uint8_t bytes[4];
  } u;

  u.integer = 0x01020408;
  if (u.bytes[0] == 0x08 && u.bytes[3] == 0x01)
    return FOUG_LITTLE_ENDIAN;
  else if (u.bytes[0] == 0x01 && u.bytes[3] == 0x08)
    return FOUG_BIG_ENDIAN;
  else if (u.bytes[1] == 0x08 && u.bytes[2] == 0x01)
    return FOUG_MIDDLE_ENDIAN;
  else
    return FOUG_OTHER_ENDIAN;
}

uint16_t foug_uint16_swap(uint16_t val)
{
  return ((val & 0x00FF) << 8) | ((val >> 8) & 0x00FF);
}

uint16_t foug_uint16_noswap(uint16_t val)
{
  return val;
}

uint16_t foug_decode_uint16_le(const uint8_t* bytes)
{
  /* |0 |1 | */
  /* |BB|AA| -> 0xAABB */
  return (bytes[1] << 8) | bytes[0];
}

uint16_t foug_decode_uint16_be(const uint8_t* bytes)
{
  /* |0 |1 | */
  /* |AA|BB| -> 0xAABB */
  return (bytes[0] << 8) | bytes[1];
}

void foug_encode_uint16_le(uint16_t val, uint8_t* bytes)
{
  _INTERNAL_FOUG_ENCODE_NUMERIC_LE(uint16_t);
}

uint32_t foug_uint32_swap(uint32_t val)
{
  return
      ((val & 0x000000FF) << 24)
      | ((val & 0x0000FF00) << 8)
      | ((val >> 8) & 0x0000FF00)
      | ((val >> 24) & 0x000000FF);
}

uint32_t foug_uint32_noswap(uint32_t val)
{
  return val;
}

uint32_t foug_decode_uint32_le(const uint8_t* bytes)
{
  /* |DD|CC|BB|AA| -> 0xAABBCCDD */
  return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

uint32_t foug_decode_uint32_me(const uint8_t* bytes)
{
  /* |DD|CC|BB|AA| -> 0xCCDDAABB */
  return (bytes[0] >> 8) | (bytes[2] << 8) | (bytes[3] << 8) | (bytes[8] << 8);
}

uint32_t foug_decode_uint32_be(const uint8_t* bytes)
{
  /* |DD|CC|BB|AA| -> 0xDDCCBBAA */
  return bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24);
}

void foug_encode_uint32_le(uint32_t val, uint8_t* bytes)
{
  _INTERNAL_FOUG_ENCODE_NUMERIC_LE(uint32_t);
}

static foug_real32_t convert_real32(uint32_t val)
{
  union
  {
    uint32_t as_integer;
    foug_real32_t as_float;
  } u;

  u.as_integer = val;
  return u.as_float;
}

foug_real32_t foug_decode_real32_le(const uint8_t* bytes)
{
  return convert_real32(foug_decode_uint32_le(bytes));
}

foug_real32_t foug_decode_real32_me(const uint8_t* bytes)
{
  return convert_real32(foug_decode_uint32_me(bytes));
}

foug_real32_t foug_decode_real32_be(const uint8_t* bytes)
{
  return convert_real32(foug_decode_uint32_be(bytes));
}

void foug_encode_real32_le(foug_real32_t val, uint8_t* bytes)
{
  _INTERNAL_FOUG_ENCODE_NUMERIC_LE(foug_real32_t);
}
