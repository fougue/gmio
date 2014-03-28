#ifndef GMIO_INTERNAL_CONVERT_H
#define GMIO_INTERNAL_CONVERT_H

#include "../global.h"

typedef union
{
  uint32_t as_uint32;
  float    as_float;
} gmio_uint32_float_t;

gmio_real32_t gmio_convert_real32(uint32_t val);
uint32_t      gmio_convert_uint32(gmio_real32_t val);

#endif /* GMIO_INTERNAL_CONVERT_H */
