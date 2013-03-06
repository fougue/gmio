#ifndef FOUG_C_ENDIAN_H
#define FOUG_C_ENDIAN_H

#include "global.h"

typedef enum
{
  FOUG_LITTLE_ENDIAN,
  FOUG_BIG_ENDIAN,
  FOUG_MIDDLE_ENDIAN,
  FOUG_OTHER_ENDIAN
} foug_endianness_t;

FOUG_LIB_EXPORT foug_endianness_t foug_host_endianness();

FOUG_LIB_EXPORT uint16_t foug_uint16_swap(uint16_t val);
FOUG_LIB_EXPORT uint16_t foug_uint16_noswap(uint16_t val);
FOUG_LIB_EXPORT uint16_t foug_decode_uint16_le(const uint8_t* bytes);
FOUG_LIB_EXPORT uint16_t foug_decode_uint16_be(const uint8_t* bytes);
FOUG_LIB_EXPORT void foug_encode_uint16_le(uint16_t val, uint8_t* bytes);

FOUG_LIB_EXPORT uint32_t foug_uint32_swap(uint32_t val);
FOUG_LIB_EXPORT uint32_t foug_uint32_noswap(uint32_t val);
FOUG_LIB_EXPORT uint32_t foug_decode_uint32_le(const uint8_t* bytes);
FOUG_LIB_EXPORT uint32_t foug_decode_uint32_me(const uint8_t* bytes);
FOUG_LIB_EXPORT uint32_t foug_decode_uint32_be(const uint8_t* bytes);
FOUG_LIB_EXPORT void foug_encode_uint32_le(uint32_t val, uint8_t* bytes);
FOUG_LIB_EXPORT void foug_encode_uint32_be(uint32_t val, uint8_t* bytes);

FOUG_LIB_EXPORT foug_real32_t foug_decode_real32_le(const uint8_t* bytes);
FOUG_LIB_EXPORT foug_real32_t foug_decode_real32_me(const uint8_t* bytes);
FOUG_LIB_EXPORT foug_real32_t foug_decode_real32_be(const uint8_t* bytes);
FOUG_LIB_EXPORT void foug_encode_real32_le(foug_real32_t val, uint8_t* bytes);

#endif /* FOUG_ENDIAN_H */
