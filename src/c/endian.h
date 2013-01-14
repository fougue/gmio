#ifndef FOUG_ENDIAN_H
#define FOUG_ENDIAN_H

#include "foug_global.h"

typedef enum
{
  FOUG_LITTLE_ENDIAN,
  FOUG_BIG_ENDIAN,
  FOUG_MIDDLE_ENDIAN,
  FOUG_OTHER_ENDIAN
} foug_endianness_t;

foug_endianness_t foug_host_endianness();

uint16_t foug_uint16_swap(uint16_t val);
uint16_t foug_uint16_noswap(uint16_t val);
uint16_t foug_decode_uint16_le(const uint8_t* bytes);
uint16_t foug_decode_uint16_be(const uint8_t* bytes);

uint32_t foug_uint32_swap(uint32_t val);
uint32_t foug_uint32_noswap(uint32_t val);
uint32_t foug_decode_uint32_le(const uint8_t* bytes);
uint32_t foug_decode_uint32_me(const uint8_t* bytes);
uint32_t foug_decode_uint32_be(const uint8_t* bytes);

foug_real32_t foug_decode_real32_le(const uint8_t* bytes);
foug_real32_t foug_decode_real32_me(const uint8_t* bytes);
foug_real32_t foug_decode_real32_be(const uint8_t* bytes);

#endif /* FOUG_ENDIAN_H */
