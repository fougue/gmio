#ifndef GMIO_ENDIAN_H
#define GMIO_ENDIAN_H

#include "global.h"

GMIO_C_LINKAGE_BEGIN

/*! This enum identifies common endianness (byte order) of computer memory */
enum gmio_endianness
{
  GMIO_LITTLE_ENDIAN,
  GMIO_BIG_ENDIAN,
  GMIO_MIDDLE_ENDIAN,
  GMIO_OTHER_ENDIAN
};

typedef enum gmio_endianness gmio_endianness_t;

/*! Returns endianness (byte order) of the host's CPU architecture */
GMIO_LIB_EXPORT gmio_endianness_t gmio_host_endianness();

GMIO_C_LINKAGE_END

#endif /* GMIO_ENDIAN_H */
