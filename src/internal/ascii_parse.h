#ifndef FOUG_INTERNAL_ASCII_PARSE_H
#define FOUG_INTERNAL_ASCII_PARSE_H

#include "../global.h"
#include "../stream.h"

typedef struct
{
  foug_stream_t* stream;
  char*          buffer;
  uint32_t       buffer_offset;
  uint32_t       buffer_size;

  void* cookie;
  void (*stream_read_hook)(void*, const char*, uint32_t);
} foug_ascii_stream_fwd_iterator_t;

typedef struct
{
  char*  data;
  size_t max_len;
  size_t len;
} foug_ascii_string_buffer_t;

void  foug_ascii_stream_fwd_iterator_init(foug_ascii_stream_fwd_iterator_t* it);
char* foug_current_char(foug_ascii_stream_fwd_iterator_t* it);
char* foug_next_char(foug_ascii_stream_fwd_iterator_t* it);
void  foug_skip_spaces(foug_ascii_stream_fwd_iterator_t* it);
int   foug_eat_string(foug_ascii_stream_fwd_iterator_t* it, foug_ascii_string_buffer_t* str_buffer);
int   foug_get_real32(const char* str, foug_real32_t* value_ptr);

#endif /* FOUG_INTERNAL_ASCII_PARSE_H */
