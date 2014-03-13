#include "convert.h"

foug_real32_t foug_convert_real32(uint32_t val)
{
  foug_uint32_float_t conv;
  conv.as_uint32 = val;
  return conv.as_float;
}

uint32_t foug_convert_uint32(foug_real32_t val)
{
  foug_uint32_float_t conv;
  conv.as_float = val;
  return conv.as_uint32;
}
