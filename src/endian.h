#ifndef FOUG_C_ENDIAN_H
#define FOUG_C_ENDIAN_H

#include "global.h"

/*! This enum identifies endian representations of numbers */
typedef enum
{
  FOUG_LITTLE_ENDIAN,
  FOUG_BIG_ENDIAN,
  FOUG_MIDDLE_ENDIAN,
  FOUG_OTHER_ENDIAN
} foug_endianness_t;

/*! Returns endianness (byte order) of the host's CPU architecture */
FOUG_LIB_EXPORT foug_endianness_t foug_host_endianness();

#endif /* FOUG_ENDIAN_H */
