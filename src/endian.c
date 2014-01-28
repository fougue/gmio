#include "endian.h"

#include "internal/convert.h"

#include <string.h>

typedef union
{
  uint32_t integer;
  uint8_t  bytes[4];
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

