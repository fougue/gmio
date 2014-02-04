#include "stream_buffer.h"

#include <string.h>

static foug_bool_t foug_stream_buffer_at_end(void* cookie)
{
  const foug_buffer_t* buff = (const foug_buffer_t*)cookie;
  return buff->pos >= buff->len;
}

static int foug_stream_buffer_error(void* cookie)
{
  const foug_buffer_t* buff = (const foug_buffer_t*)cookie;
  return buff == NULL || buff->pos > buff->len;
}

static size_t foug_stream_buffer_read(void* cookie,
                                      void* ptr,
                                      size_t item_size,
                                      size_t item_count)
{
  if (item_size > 0 && item_count > 0) {
    foug_buffer_t* buff = (foug_buffer_t*)cookie;
    const size_t buff_remaining_size = buff->len - buff->pos;
    const size_t wanted_read_size = item_size * item_count;
    const size_t next_read_size = wanted_read_size <= buff_remaining_size ? wanted_read_size :
                                                                            buff_remaining_size;
    const size_t next_read_item_count = next_read_size / item_size;

    memcpy(ptr, (const char*)buff->ptr + buff->pos, next_read_item_count * item_size);
    buff->pos += next_read_item_count * item_size;
    return next_read_item_count;
  }
  else {
    return 0;
  }
}

static size_t foug_stream_buffer_write(void* cookie,
                                       const void* ptr,
                                       size_t item_size,
                                       size_t item_count)
{
  if (item_size > 0 && item_count > 0) {
    foug_buffer_t* buff = (foug_buffer_t*)cookie;
    const size_t buff_remaining_size = buff->len - buff->pos;
    const size_t wanted_write_size = item_size * item_count;
    const size_t next_write_size = wanted_write_size <= buff_remaining_size ? wanted_write_size :
                                                                              buff_remaining_size;
    const size_t next_write_item_count = next_write_size / item_size;

    memcpy((char*)buff->ptr + buff->pos, ptr, next_write_item_count * item_size);
    buff->pos += next_write_item_count * item_size;
    return next_write_item_count;
  }
  else {
    return 0;
  }
}

void foug_stream_set_buffer(foug_stream_t *stream, foug_buffer_t *buff)
{
  stream->cookie = buff;
  stream->at_end_func = foug_stream_buffer_at_end;
  stream->error_func = foug_stream_buffer_error;
  stream->read_func = foug_stream_buffer_read;
  stream->write_func = foug_stream_buffer_write;
}
