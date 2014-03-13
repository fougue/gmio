#ifndef FOUG_INTERNAL_ASCII_PARSE_H
#define FOUG_INTERNAL_ASCII_PARSE_H

#include "../global.h"
#include "../stream.h"

typedef struct
{
  char*  ptr;       /*!< Buffer contents */
  size_t len;       /*!< Size(length) of current contents */
  size_t max_len;   /*!< Maximum contents size(length) */
} foug_ascii_string_buffer_t;

typedef struct
{
  foug_stream_t*             stream;
  foug_ascii_string_buffer_t buffer;
  size_t                     buffer_pos; /*!< Position indicator in buffer */

  void* cookie;
  void (*stream_read_hook)(void*, const foug_ascii_string_buffer_t*);
} foug_ascii_stream_fwd_iterator_t;

void  foug_ascii_stream_fwd_iterator_init(foug_ascii_stream_fwd_iterator_t* it);
char* foug_current_char(const foug_ascii_stream_fwd_iterator_t* it);
void  foug_skip_spaces(foug_ascii_stream_fwd_iterator_t* it);
int   foug_eat_word(foug_ascii_stream_fwd_iterator_t* it, foug_ascii_string_buffer_t* buffer);
int   foug_get_real32(const char* str, foug_real32_t* value_ptr);

char*       foug_next_char(foug_ascii_stream_fwd_iterator_t* it);
foug_bool_t foug_checked_next_chars(foug_ascii_stream_fwd_iterator_t* it, const char* str);

#endif /* FOUG_INTERNAL_ASCII_PARSE_H */
