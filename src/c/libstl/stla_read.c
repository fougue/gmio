#include "stla_read.h"

#include "../error.h"

#include <ctype.h> /* isspace() */
#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef struct foug_stream_fwd_iterator
{
  foug_stream_t* stream;
  char* buffer;
  uint32_t buffer_offset;
  uint32_t buffer_size;

  void* cookie;
  void (*stream_read_hook)(struct foug_stream_fwd_iterator* it);
} foug_stream_fwd_iterator_t;

typedef struct foug_stream_fwd_iterator_stla_cookie
{
  foug_task_control_t* task_control;
  size_t stream_data_size;
  size_t stream_offset;
  foug_bool_t is_stop_requested;
} foug_stream_fwd_iterator_stla_cookie_t;

static void foug_stream_fwd_iterator_stla_read_hook(foug_stream_fwd_iterator_t* it)
{
  foug_stream_fwd_iterator_stla_cookie_t* cookie =
      it != NULL ? (foug_stream_fwd_iterator_stla_cookie_t*)(it->cookie) : NULL;
  if (cookie != NULL) {
    const uint8_t progress_pc = foug_percentage(0, cookie->stream_data_size, cookie->stream_offset);
    cookie->is_stop_requested = !foug_task_control_handle_progress(cookie->task_control, progress_pc);
    cookie->stream_offset += it->buffer_size;
  }
}

static char* current_char(foug_stream_fwd_iterator_t* it)
{
  if (it == NULL)
    return NULL;

  if (it->buffer_offset < it->buffer_size)
    return it->buffer + it->buffer_offset;
  return NULL;
}

static char* next_char(foug_stream_fwd_iterator_t* it)
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
        it->stream_read_hook(it);
      return it->buffer;
    }
  }
}

void foug_stream_fwd_iterator_init(foug_stream_fwd_iterator_t* it)
{
  it->buffer_offset = it->buffer_size; /* This will cause the first call to foug_stream_read() */
  next_char(it);
}

static void skip_spaces(foug_stream_fwd_iterator_t* it)
{
  char* curr_char = current_char(it);
  while (curr_char != NULL && isspace(*curr_char))
    curr_char = next_char(it);
}

static int eat_token(foug_stream_fwd_iterator_t* it, const char* token)
{
  uint32_t i;
  const char* stream_curr_char;

  if (token == NULL || current_char(it) == NULL)
    return -1;

  skip_spaces(it);
  stream_curr_char = current_char(it);

  for (i = 0; token[i] != 0 && stream_curr_char != NULL; ++i) {
    if (token[i] != *stream_curr_char)
      return -2;
    stream_curr_char = next_char(it);
  }

  return token[i] != 0 ? -3 : 0;
}

typedef struct foug_string_buffer
{
  char* data;
  size_t max_len;
  size_t len;
} foug_string_buffer_t;

static int eat_string(foug_stream_fwd_iterator_t* it, foug_string_buffer_t* str_buffer)
{
  size_t i;
  const char* stream_curr_char;
  int isspace_res;

  if (str_buffer == NULL || str_buffer->data == NULL || str_buffer->max_len == 0)
    return -1;

  str_buffer->len = 0;
  skip_spaces(it);
  stream_curr_char = current_char(it);

  isspace_res = 0;
  i = 0;
  while (i < str_buffer->max_len && stream_curr_char != NULL && !isspace_res) {
    isspace_res = isspace(*stream_curr_char);
    if (!isspace_res) {
      str_buffer->data[i] = *stream_curr_char;
      stream_curr_char = next_char(it);
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

static int read_real32(foug_stream_fwd_iterator_t* it,
                       foug_string_buffer_t* str_buffer,
                       foug_real32_t* value_ptr)
{
  char* conv_end_ptr; /* for strtod() */

  if (eat_string(it, str_buffer) != 0)
    return -1;
  *value_ptr = (foug_real32_t)strtod(str_buffer->data, &conv_end_ptr);
  if (conv_end_ptr == str_buffer->data || errno == ERANGE)
    return -2;

  return 0;
}

static int read_coords(foug_stream_fwd_iterator_t* it,
                       foug_string_buffer_t* str_buffer,
                       foug_stl_coords_t* coords_ptr)
{
  if (read_real32(it, str_buffer, &coords_ptr->x) != 0)
    return -1;
  if (read_real32(it, str_buffer, &coords_ptr->y) != 0)
    return -1;
  if (read_real32(it, str_buffer, &coords_ptr->z) != 0)
    return -1;
  return 0;
}

static int eat_facet(foug_stream_fwd_iterator_t* it,
                     foug_string_buffer_t* str_buffer,
                     foug_stl_triangle_t* triangle)
{
  /* Try to eat "facet" */
  if (eat_string(it, str_buffer) != 0)
    return -1;
  if (strcmp(str_buffer->data, "facet") != 0) /* Maybe "endsolid" */
    return 1;

  /* Read normal */
  if (eat_token(it, "normal") != 0)
    return -2;
  if (read_coords(it, str_buffer, &triangle->normal) != 0)
    return -3;

  /* Try to eat "outer loop" */
  if (eat_token(it, "outer") != 0)
    return -2;
  if (eat_token(it, "loop") != 0)
    return -2;

  /* Read vertex 1 */
  if (eat_token(it, "vertex") != 0)
    return -2;
  if (read_coords(it, str_buffer, &triangle->v1) != 0)
    return -3;

  /* Read vertex 2 */
  if (eat_token(it, "vertex") != 0)
    return -2;
  if (read_coords(it, str_buffer, &triangle->v2) != 0)
    return -3;

  /* Read vertex 3 */
  if (eat_token(it, "vertex") != 0)
    return -2;
  if (read_coords(it, str_buffer, &triangle->v3) != 0)
    return -3;

  /* Try to eat "endloop" */
  if (eat_token(it, "endloop") != 0)
    return -2;

  /* Try to eat "endfacet" */
  if (eat_token(it, "endfacet") != 0)
    return -2;

  return 0;
}

#define FOUG_STLA_READ_STRING_BUFFER_LEN    512

int foug_stla_read(foug_stla_read_args_t *args)
{
  char fixed_buffer[FOUG_STLA_READ_STRING_BUFFER_LEN];
  foug_string_buffer_t string_buffer;
  foug_stream_fwd_iterator_t it;
  foug_stream_fwd_iterator_stla_cookie_t stla_cookie;

  if (args->buffer == NULL)
    return FOUG_DATAX_NULL_BUFFER_ERROR;
  if (args->buffer_size == 0)
    return FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;

  stla_cookie.task_control = &args->task_control;
  stla_cookie.stream_data_size = args->data_size_hint;
  stla_cookie.stream_offset = 0;
  stla_cookie.is_stop_requested = 0;

  it.stream = &(args->stream);
  it.buffer = args->buffer;
  it.buffer_offset = 0;
  it.buffer_size = args->buffer_size;
  it.cookie = &stla_cookie;
  it.stream_read_hook = foug_stream_fwd_iterator_stla_read_hook;
  foug_stream_fwd_iterator_init(&it);

  string_buffer.data = fixed_buffer;
  string_buffer.max_len = FOUG_STLA_READ_STRING_BUFFER_LEN;
  string_buffer.len = 0;

  /* Eat solids */
  while (eat_token(&it, "solid") == 0 && !stla_cookie.is_stop_requested) {
    int eat_facet_result;
    foug_stl_triangle_t triangle;

    /* Try to eat solid's name */
    if (eat_string(&it, &string_buffer) != 0)
      return FOUG_STLA_READ_PARSE_ERROR;
    if (args->geom_input.begin_solid_func != NULL)
      args->geom_input.begin_solid_func(&args->geom_input, string_buffer.data);

    /* Try to eat facets */
    while ((eat_facet_result = eat_facet(&it, &string_buffer, &triangle)) >= 0
           && !stla_cookie.is_stop_requested)
    {
      if (eat_facet_result == 0) {
        if (args->geom_input.process_next_triangle_func != NULL)
          args->geom_input.process_next_triangle_func(&args->geom_input, &triangle);
      }
      else {
        break; /* Ate "endsolid" */
      }
    } /* end while */

    /* Handle "endsolid" */
    if (eat_facet_result > 0) {
      if (eat_string(&it, &string_buffer) != 0)
        return FOUG_STLA_READ_PARSE_ERROR;
      if (args->geom_input.end_solid_func != NULL)
        args->geom_input.end_solid_func(&args->geom_input, string_buffer.data);
    }
    else {
      return FOUG_STLA_READ_PARSE_ERROR;
    }
  }

  return !stla_cookie.is_stop_requested ? FOUG_DATAX_NO_ERROR : FOUG_DATAX_TASK_STOPPED_ERROR;
}
