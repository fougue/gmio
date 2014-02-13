#ifndef FOUG_LIBSTL_FORMAT_H
#define FOUG_LIBSTL_FORMAT_H

#include "stl_global.h"
#include "../stream.h"

enum foug_stl_format
{
  FOUG_STL_ASCII_FORMAT,     /*!< STL ASCII (text) */
  FOUG_STL_BINARY_LE_FORMAT, /*!< STL binary (little-endian) */
  FOUG_STL_BINARY_BE_FORMAT, /*!< STL binary (big-endian) */
  FOUG_STL_UNKNOWN_FORMAT
};

typedef enum foug_stl_format foug_stl_format_t;

FOUG_DATAX_LIBSTL_EXPORT
foug_stl_format_t foug_stl_get_format(foug_stream_t* stream, size_t data_size);

#endif /* FOUG_LIBSTL_FORMAT_H */
