#ifndef FOUG_C_STREAM_H
#define FOUG_C_STREAM_H

#include "global.h"
#include "memory.h"
#include <stdio.h>

/* foug_stream */
typedef struct foug_stream foug_stream_t;
struct foug_stream
{
  void* cookie;
  foug_bool_t (*at_end_func)(foug_stream_t*);
  int32_t (*error_func)(foug_stream_t*);
  size_t (*read_func)(foug_stream_t*, void*, size_t, size_t);
  size_t (*write_func)(foug_stream_t*, const void*, size_t, size_t);
};

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
