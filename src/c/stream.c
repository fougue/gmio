#include "stream.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct _internal_foug_stream
{
  void* cookie;
  foug_stream_manip_t manip;
};

foug_stream_t* foug_stream_create(foug_malloc_func_t func, void* data, foug_stream_manip_t manip)
{
  if (func == NULL)
    return NULL;
  foug_stream_t* stream = (*func)(sizeof(struct _internal_foug_stream));
  if (stream != NULL) {
    stream->cookie = data;
    stream->manip = manip;
  }
  return stream;
}

foug_stream_manip_t foug_stream_manip_null()
{
  foug_stream_manip_t manip;
  memset(&manip, 0, sizeof(foug_stream_manip_t));
  return manip;
}

static foug_bool_t foug_stream_stdio_at_end(foug_stream_t* stream)
{
  return feof((FILE*) stream->cookie);
}

static int32_t foug_stream_stdio_error(foug_stream_t* stream)
{
  return ferror((FILE*) stream->cookie);
}

static size_t foug_stream_stdio_read(foug_stream_t* stream,
                                     void* ptr,
                                     size_t item_size,
                                     size_t item_count)
{
  return fread(ptr, item_size, item_count, (FILE*) stream->cookie);
}

static size_t foug_stream_stdio_write(foug_stream_t* stream,
                                      const void* ptr,
                                      size_t item_size,
                                      size_t item_count)
{
  return fwrite(ptr, item_size, item_count, (FILE*) stream->cookie);
}

foug_stream_manip_t foug_stream_manip_stdio()
{
  foug_stream_manip_t manip;
  manip.at_end_func = foug_stream_stdio_at_end;
  manip.error_func = foug_stream_stdio_error;
  manip.read_func = foug_stream_stdio_read;
  manip.write_func = foug_stream_stdio_write;
  return manip;
}

foug_bool_t foug_stream_at_end(foug_stream_t* stream)
{
  if (stream != NULL && stream->manip.at_end_func != NULL)
    return (*(stream->manip.at_end_func))(stream);
  return 0;
}

int foug_stream_error(foug_stream_t* stream)
{
  if (stream != NULL && stream->manip.error_func != NULL)
    return (*(stream->manip.error_func))(stream);
  return 0;
}

size_t foug_stream_read(foug_stream_t* stream, void *ptr, size_t item_size, size_t item_count)
{
  if (stream != NULL && stream->manip.read_func != NULL)
    return (*(stream->manip.read_func))(stream, ptr, item_size, item_count);
  return 0;
}

size_t foug_stream_write(foug_stream_t* stream, const void *ptr, size_t item_size, size_t item_count)
{
  if (stream != NULL && stream->manip.write_func != NULL)
    return (*(stream->manip.write_func))(stream, ptr, item_size, item_count);
  return 0;
}

void* foug_stream_get_cookie(const foug_stream_t* stream)
{
  return stream != NULL ? stream->cookie : NULL;
}
