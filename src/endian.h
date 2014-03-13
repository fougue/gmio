#ifndef FOUG_C_ENDIAN_H
#define FOUG_C_ENDIAN_H

#include "global.h"

FOUG_C_LINKAGE_BEGIN

/*! This enum identifies endian representations of numbers */
enum foug_endianness
{
  FOUG_LITTLE_ENDIAN,
  FOUG_BIG_ENDIAN,
  FOUG_MIDDLE_ENDIAN,
  FOUG_OTHER_ENDIAN
};

typedef enum foug_endianness foug_endianness_t;

/*! Returns endianness (byte order) of the host's CPU architecture */
FOUG_LIB_EXPORT foug_endianness_t foug_host_endianness();

FOUG_C_LINKAGE_END

#endif /* FOUG_ENDIAN_H */
