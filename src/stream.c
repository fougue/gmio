#include "stream.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void foug_stream_set_null(foug_stream_t* stream)
{
  memset(stream, 0, sizeof(foug_stream_t));
}

static foug_bool_t foug_stream_stdio_at_end(void* cookie)
{
  return feof((FILE*) cookie);
}

static int foug_stream_stdio_error(void* cookie)
{
  return ferror((FILE*) cookie);
}

static size_t foug_stream_stdio_read(void* cookie,
                                     void* ptr,
                                     size_t item_size,
                                     size_t item_count)
{
  return fread(ptr, item_size, item_count, (FILE*) cookie);
}

static size_t foug_stream_stdio_write(void* cookie,
                                      const void* ptr,
                                      size_t item_size,
                                      size_t item_count)
{
  return fwrite(ptr, item_size, item_count, (FILE*) cookie);
}

void foug_stream_set_stdio(foug_stream_t* stream, FILE* file)
{
  stream->cookie = file;
  stream->at_end_func = foug_stream_stdio_at_end;
  stream->error_func = foug_stream_stdio_error;
  stream->read_func = foug_stream_stdio_read;
  stream->write_func = foug_stream_stdio_write;
}

foug_bool_t foug_stream_at_end(foug_stream_t* stream)
{
  if (stream != NULL && stream->at_end_func != NULL)
    return stream->at_end_func(stream->cookie);
  return 0;
}

int foug_stream_error(foug_stream_t* stream)
{
  if (stream != NULL && stream->error_func != NULL)
    return stream->error_func(stream->cookie);
  return 0;
}

size_t foug_stream_read(foug_stream_t* stream, void *ptr, size_t size, size_t count)
{
  if (stream != NULL && stream->read_func != NULL)
    return stream->read_func(stream->cookie, ptr, size, count);
  return 0;
}

size_t foug_stream_write(foug_stream_t* stream, const void *ptr, size_t size, size_t count)
{
  if (stream != NULL && stream->write_func != NULL)
    return stream->write_func(stream->cookie, ptr, size, count);
  return 0;
}
