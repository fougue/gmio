#ifndef GMIO_INTERNAL_ASCII_PARSE_H
#define GMIO_INTERNAL_ASCII_PARSE_H

#include "../global.h"
#include "../stream.h"

struct gmio_ascii_string_buffer
{
  char*  ptr;       /*!< Buffer contents */
  size_t len;       /*!< Size(length) of current contents */
  size_t max_len;   /*!< Maximum contents size(length) */
};

typedef struct gmio_ascii_string_buffer  gmio_ascii_string_buffer_t;

struct gmio_ascii_stream_fwd_iterator
{
  gmio_stream_t*             stream;
  gmio_ascii_string_buffer_t buffer;
  size_t                     buffer_pos; /*!< Position indicator in buffer */

  void* cookie;
  void (*stream_read_hook)(void* cookie, const gmio_ascii_string_buffer_t* str_buffer);
};

typedef struct gmio_ascii_stream_fwd_iterator  gmio_ascii_stream_fwd_iterator_t;

void  gmio_ascii_stream_fwd_iterator_init(gmio_ascii_stream_fwd_iterator_t* it);
char* gmio_current_char(const gmio_ascii_stream_fwd_iterator_t* it);
void  gmio_skip_spaces(gmio_ascii_stream_fwd_iterator_t* it);
int   gmio_eat_word(gmio_ascii_stream_fwd_iterator_t* it, gmio_ascii_string_buffer_t* buffer);
int   gmio_get_real32(const char* str, gmio_real32_t* value_ptr);

char*       gmio_next_char(gmio_ascii_stream_fwd_iterator_t* it);
gmio_bool_t gmio_checked_next_chars(gmio_ascii_stream_fwd_iterator_t* it, const char* str);

#endif /* GMIO_INTERNAL_ASCII_PARSE_H */