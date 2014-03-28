#include "convert.h"

gmio_real32_t gmio_convert_real32(uint32_t val)
{
  gmio_uint32_float_t conv;
  conv.as_uint32 = val;
  return conv.as_float;
}

uint32_t gmio_convert_uint32(gmio_real32_t val)
{
  gmio_uint32_float_t conv;
  conv.as_float = val;
  return conv.as_uint32;
}
