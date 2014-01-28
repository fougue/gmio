#ifndef FOUG_INTERNAL_CONVERT_H
#define FOUG_INTERNAL_CONVERT_H

#include "../global.h"

typedef union
{
  uint32_t as_uint32;
  float    as_float;
} foug_uint32_float_t;

foug_real32_t foug_convert_real32(uint32_t val);
uint32_t      foug_convert_uint32(foug_real32_t val);

#endif /* FOUG_INTERNAL_CONVERT_H */
