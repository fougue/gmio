#include "ascii_parse.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

void foug_ascii_stream_fwd_iterator_init(foug_ascii_stream_fwd_iterator_t *it)
{
  /* Trick: declaring the buffer exhausted will actually trigger the first call to
   * foug_stream_read() inside foug_next_char()
   */
  it->buffer.len = 0;
  it->buffer_pos = it->buffer.max_len;
  foug_next_char(it);
}

char *foug_current_char(const foug_ascii_stream_fwd_iterator_t *it)
{
  if (it != NULL && it->buffer_pos < it->buffer.len)
    return it->buffer.ptr + it->buffer_pos;
  return NULL;
}

char *foug_next_char(foug_ascii_stream_fwd_iterator_t *it)
{
  if (it == NULL)
    return NULL;

  if ((it->buffer_pos + 1) < it->buffer.len) {
    ++(it->buffer_pos);
    return it->buffer.ptr + it->buffer_pos;
  }
  else {
    if (foug_stream_error(it->stream) != 0 || foug_stream_at_end(it->stream))
      return NULL;

    /* Read next chunk of data */
    it->buffer_pos = 0;
    it->buffer.len = foug_stream_read(it->stream, it->buffer.ptr, sizeof(char), it->buffer.max_len);
    if (foug_stream_error(it->stream) == 0) {
      if (it->stream_read_hook != NULL)
        it->stream_read_hook(it->cookie, &it->buffer);
      return it->buffer.ptr;
    }
  }

  return NULL;
}

void foug_skip_spaces(foug_ascii_stream_fwd_iterator_t *it)
{
  const char* curr_char = foug_current_char(it);
  while (curr_char != NULL && isspace(*curr_char))
    curr_char = foug_next_char(it);
}

int foug_eat_word(foug_ascii_stream_fwd_iterator_t *it, foug_ascii_string_buffer_t *buffer)
{
  const char* stream_curr_char = NULL;
  int isspace_res = 0;
  size_t i = 0;

  if (buffer == NULL || buffer->ptr == NULL)
    return -1;

  buffer->len = 0;
  foug_skip_spaces(it);
  stream_curr_char = foug_current_char(it);

  while (i < buffer->max_len && stream_curr_char != NULL && isspace_res == 0) {
    isspace_res = isspace(*stream_curr_char);
    if (isspace_res == 0) {
      buffer->ptr[i] = *stream_curr_char;
      stream_curr_char = foug_next_char(it);
      ++i;
    }
  }

  if (i < buffer->max_len) {
    buffer->ptr[i] = 0; /* End string with terminating null byte */
    buffer->len = i;
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

foug_bool_t foug_checked_next_chars(foug_ascii_stream_fwd_iterator_t *it, const char *str)
{
  size_t pos = 0;
  const char* curr_char = foug_current_char(it);
  foug_bool_t same = curr_char != NULL && *curr_char == *str;

  while (same) {
    curr_char = foug_next_char(it);
    same = curr_char != NULL && *curr_char == str[++pos];
  }

  return same;
}
