#ifndef GMIO_LIBSTL_FORMAT_H
#define GMIO_LIBSTL_FORMAT_H

#include "stl_global.h"
#include "../gmio_core/stream.h"

GMIO_C_LINKAGE_BEGIN

/*! This enums defines the various STL formats */
enum gmio_stl_format
{
  GMIO_STL_ASCII_FORMAT,     /*!< STL ASCII (text) */
  GMIO_STL_BINARY_LE_FORMAT, /*!< STL binary (little-endian) */
  GMIO_STL_BINARY_BE_FORMAT, /*!< STL binary (big-endian) */
  GMIO_STL_UNKNOWN_FORMAT
};

typedef enum gmio_stl_format gmio_stl_format_t;

GMIO_LIBSTL_EXPORT
gmio_stl_format_t gmio_stl_get_format(gmio_stream_t* stream, size_t data_size);

GMIO_C_LINKAGE_END

#endif /* GMIO_LIBSTL_FORMAT_H */
