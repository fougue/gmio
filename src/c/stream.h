#ifndef FOUG_C_STREAM_H
#define FOUG_C_STREAM_H

#include "foug_global.h"
#include "memory.h"

/* foug_stream : opaque structure */
typedef struct _internal_foug_stream foug_stream_t;

/* foug_stream_manip */
typedef foug_bool (*foug_stream_at_end_func_t)(foug_stream_t*);
typedef foug_int32 (*foug_stream_seek_func_t)(foug_stream_t*, foug_int64);
typedef foug_uint64 (*foug_stream_read_func_t)(foug_stream_t*, char*, foug_uint64);
typedef foug_uint64 (*foug_stream_write_func_t)(foug_stream_t*, const char*, foug_uint64);
typedef struct foug_stream_manip
{
  foug_stream_at_end_func_t at_end_func;
  foug_stream_seek_func_t seek_func;
  foug_stream_read_func_t read_func;
  foug_stream_write_func_t write_func;
} foug_stream_manip_t;

foug_stream_manip_t foug_stream_manip_null();
foug_stream_manip_t foug_stream_manip_stdio();

/* Services */
foug_stream_t* foug_stream_create(foug_malloc_func_t func, void* data, foug_stream_manip_t manip);

foug_bool foug_stream_at_end(foug_stream_t* stream);
foug_int32 foug_stream_seek(foug_stream_t* stream, foug_int64 max_size);
foug_uint64 foug_stream_read(foug_stream_t* stream, char* s, foug_uint64 max_size);
foug_uint64 foug_stream_write(foug_stream_t* stream, const char* s, foug_uint64 max_size);
void* foug_stream_get_cookie(const foug_stream_t* stream);

#endif /* FOUG_C_STREAM_H */
