#ifndef FOUG_C_STREAM_H
#define FOUG_C_STREAM_H

#include "global.h"
#include "memory.h"
#include <stdio.h>

typedef struct foug_stream foug_stream_t;

/*! \brief Stream that can get input from an arbitrary data source or can write output to an
 *         arbitrary data sink
 *
 *  This is pretty much the same as
 *  <a href="http://www.gnu.org/software/libc/manual/html_mono/libc.html#Custom-Streams">
 *  custom streams</a> in the GNU C Library
 *
 * It uses a cookie being basically an opaque pointer on a hidden data type. The custom stream is
 * implemented by defining hook functions that know how to read/write the data.
 *
 */
struct foug_stream
{
  void* cookie;

  foug_bool_t (*at_end_func)(foug_stream_t*);
  int32_t     (*error_func)(foug_stream_t*);
  size_t      (*read_func)(foug_stream_t*, void*, size_t, size_t);
  size_t      (*write_func)(foug_stream_t*, const void*, size_t, size_t);
};

/* Initialization */

FOUG_LIB_EXPORT void foug_stream_set_null(foug_stream_t* stream);
FOUG_LIB_EXPORT void foug_stream_set_stdio(foug_stream_t* stream, FILE* file);

/* Services */

FOUG_LIB_EXPORT foug_bool_t foug_stream_at_end(foug_stream_t* stream);

FOUG_LIB_EXPORT int foug_stream_error(foug_stream_t* stream);

FOUG_LIB_EXPORT size_t foug_stream_read(foug_stream_t* stream,
                                        void* ptr,
                                        size_t item_size,
                                        size_t item_count);

FOUG_LIB_EXPORT size_t foug_stream_write(foug_stream_t* stream,
                                         const void* ptr,
                                         size_t item_size,
                                         size_t item_count);

#endif /* FOUG_C_STREAM_H */
