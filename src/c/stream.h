#ifndef FOUG_C_STREAM_H
#define FOUG_C_STREAM_H

#include "global.h"
#include "memory.h"

/* foug_stream : opaque structure */
typedef struct _internal_foug_stream foug_stream_t;

/* foug_stream_manip */
typedef struct foug_stream_manip
{
  foug_bool_t (*at_end_func)(foug_stream_t*);
  int32_t (*error_func)(foug_stream_t*);
  size_t (*read_func)(foug_stream_t*, void*, size_t, size_t);
  size_t (*write_func)(foug_stream_t*, const void*, size_t, size_t);
} foug_stream_manip_t;

FOUG_LIB_EXPORT foug_stream_manip_t foug_stream_manip_null();
FOUG_LIB_EXPORT foug_stream_manip_t foug_stream_manip_stdio();

/* Services */

FOUG_LIB_EXPORT foug_stream_t* foug_stream_create(foug_malloc_func_t func,
                                                  void* data,
                                                  foug_stream_manip_t manip);

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

FOUG_LIB_EXPORT void* foug_stream_get_cookie(const foug_stream_t* stream);

FOUG_LIB_EXPORT void foug_stream_set_cookie(foug_stream_t* stream, void* data);

#endif /* FOUG_C_STREAM_H */
