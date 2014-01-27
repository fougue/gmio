#include "ascii_parse.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

char *foug_current_char(foug_ascii_stream_fwd_iterator_t *it)
{
  if (it != NULL && it->buffer_offset < it->buffer_size)
    return it->buffer + it->buffer_offset;
  return NULL;
}

char *foug_next_char(foug_ascii_stream_fwd_iterator_t *it)
{
  if (it == NULL)
    return NULL;

  if ((it->buffer_offset + 1) < it->buffer_size) {
    ++(it->buffer_offset);
    return it->buffer + it->buffer_offset;
  }
  else {
    size_t char_count_read;

    if (foug_stream_error(it->stream) != 0 || foug_stream_at_end(it->stream))
      return NULL;

    /* Read next chunk of data */
    char_count_read = foug_stream_read(it->stream, it->buffer, sizeof(char), it->buffer_size);
    if (foug_stream_error(it->stream) != 0) {
      return NULL;
    }
    else {
      it->buffer_offset = 0;
      it->buffer_size = char_count_read;
      if (it->stream_read_hook != NULL)
        it->stream_read_hook(it->cookie, it->buffer, it->buffer_size);
      return it->buffer;
    }
  }
}

void foug_stream_fwd_iterator_init(foug_ascii_stream_fwd_iterator_t *it)
{
  it->buffer_offset = it->buffer_size; /* This will cause the first call to foug_stream_read() */
  foug_next_char(it);
}

void foug_skip_spaces(foug_ascii_stream_fwd_iterator_t *it)
{
  const char* curr_char = foug_current_char(it);
  while (curr_char != NULL && isspace(*curr_char))
    curr_char = foug_next_char(it);
}

int foug_eat_string(foug_ascii_stream_fwd_iterator_t *it, foug_ascii_string_buffer_t *str_buffer)
{
  const char* stream_curr_char = NULL;
  int isspace_res = 0;
  size_t i = 0;

  if (str_buffer == NULL || str_buffer->data == NULL || str_buffer->max_len == 0)
    return -1;

  str_buffer->len = 0;
  foug_skip_spaces(it);
  stream_curr_char = foug_current_char(it);

  while (i < str_buffer->max_len && stream_curr_char != NULL && isspace_res == 0) {
    isspace_res = isspace(*stream_curr_char);
    if (isspace_res == 0) {
      str_buffer->data[i] = *stream_curr_char;
      stream_curr_char = foug_next_char(it);
      ++i;
    }
  }

  if (i < str_buffer->max_len) {
    str_buffer->data[i] = 0; /* End string with null terminator */
    str_buffer->len = i ;
    if (stream_curr_char != NULL || foug_stream_at_end(it->stream))
      return 0;
    return -2;
  }
  return -3;
}

int foug_get_real32(const char *str, foug_real32_t *value_ptr)
{
  char* end_ptr; /* for strtod() */

#ifdef FOUG_HAVE_STRTOF_FUNC
  *value_ptr = strtof(str, &end_ptr); /* Requires C99 */
#else
  *value_ptr = (foug_real32_t)strtod(str, &end_ptr);
#endif

  if (end_ptr == str || errno == ERANGE)
    return -1;

  return 0;
}
