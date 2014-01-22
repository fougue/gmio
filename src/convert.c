#include "convert.h"

typedef union
{
  uint32_t      as_integer;
  foug_real32_t as_float;
} _internal_foug_int_real_32_convert_t;

foug_real32_t foug_convert_real32(uint32_t val)
{
  _internal_foug_int_real_32_convert_t conv;
  conv.as_integer = val;
  return conv.as_float;
}

uint32_t foug_convert_uint32(foug_real32_t val)
{
  _internal_foug_int_real_32_convert_t conv;
  conv.as_float = val;
  return conv.as_integer;
}
