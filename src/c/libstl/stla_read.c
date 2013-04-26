#include "stla_read.h"

#include "../error.h"

#include <ctype.h> /* isspace() */
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* foug_stream_fwd_iterator */
struct foug_stream_fwd_iterator
{
  foug_stream_t* stream;
  char* buffer;
  uint32_t buffer_offset;
  uint32_t buffer_size;

  void* cookie;
  void (*stream_read_hook)(struct foug_stream_fwd_iterator* it);
};
typedef struct foug_stream_fwd_iterator
        foug_stream_fwd_iterator_t;

/* foug_string_buffer */
struct foug_string_buffer
{
  char* data;
  size_t max_len;
  size_t len;
};
typedef struct foug_string_buffer
        foug_string_buffer_t;

/* foug_stream_fwd_iterator_stla_cookie */
struct foug_stream_fwd_iterator_stla_cookie
{
  foug_task_control_t* task_control;
  size_t stream_data_size;
  size_t stream_offset;
  foug_bool_t is_stop_requested;
};
typedef struct foug_stream_fwd_iterator_stla_cookie
        foug_stream_fwd_iterator_stla_cookie_t;

/* foug_stla_token */
enum foug_stla_token
{
  ENDFACET_token,
  ENDLOOP_token,
  ENDSOLID_token,
  FACET_token,
  FLOAT_token,
  ID_token,
  LOOP_token,
  NORMAL_token,
  OUTER_token,
  SOLID_token,
  VERTEX_token,
  unknown_token
};
typedef enum foug_stla_token foug_stla_token_t;

/* foug_stla_parse_data */
struct foug_stla_parse_data
{
  foug_stla_token_t token;
  foug_bool_t  error;
  foug_stream_fwd_iterator_t             stream_iterator;
  foug_stream_fwd_iterator_stla_cookie_t stream_iterator_cookie;
  foug_string_buffer_t                   string_buffer;
  foug_stla_geom_input_t*                geom;
};
typedef struct foug_stla_parse_data
        foug_stla_parse_data_t;

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

static void foug_stream_fwd_iterator_init(foug_stream_fwd_iterator_t* it)
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

static int get_real32(const char* str, foug_real32_t* value_ptr)
{
  char* conv_end_ptr; /* for strtod() */

  *value_ptr = (foug_real32_t)strtod(str, &conv_end_ptr);
  if (conv_end_ptr == str || errno == ERANGE)
    return -1;

  return 0;
}

foug_bool_t parsing_can_continue(const foug_stla_parse_data_t* data)
{
  if (data->error || data->stream_iterator_cookie.is_stop_requested)
    return 0;
  return 1;
}

static const char* current_token_as_identifier(const foug_stla_parse_data_t* data)
{
  return data->token == ID_token ? data->string_buffer.data : "";
}

static int get_current_token_as_real32(const foug_stla_parse_data_t* data, foug_real32_t* value)
{
  if (data->token == FLOAT_token)
    return get_real32(data->string_buffer.data, value);
  return -3;
}

static void parsing_error(foug_stla_parse_data_t* data)
{
  data->error = 1;
  data->token = unknown_token;
}

static void parsing_advance(foug_stla_parse_data_t* data)
{
  const char* str = data->string_buffer.data;

  if (!parsing_can_continue(data))
    return;

  data->token = unknown_token;
  if (eat_string(&data->stream_iterator, &data->string_buffer) == 0) {
    const size_t str_len = data->string_buffer.len;

    if (str_len >= 7 && strncmp(str, "end", 3) == 0) { /* Might be "end..." token */
      switch (str[3]) {
      case 'f':
        if (strcmp(str + 4, "acet") == 0)
          data->token = ENDFACET_token;
        break;
      case 'l':
        if (strcmp(str + 4, "oop") == 0)
          data->token = ENDLOOP_token;
        break;
      case 's':
        if (strcmp(str + 4, "olid") == 0)
          data->token = ENDSOLID_token;
        break;
      default:
        data->token = ID_token;
      } /* end switch */
    }
    else if (str_len >= 4) {
      switch (str[0]) {
      case 'f':
        if (strcmp(str + 1, "acet") == 0)
          data->token = FACET_token;
        break;
      case 'l':
        if (strcmp(str + 1, "oop") == 0)
          data->token = LOOP_token;
        break;
      case 'n':
        if (strcmp(str + 1, "ormal") == 0)
          data->token = NORMAL_token;
        break;
      case 'o':
        if (strcmp(str + 1, "uter") == 0)
          data->token = OUTER_token;
        break;
      case 's':
        if (strcmp(str + 1, "olid") == 0)
          data->token = SOLID_token;
        break;
      case 'v':
        if (strcmp(str + 1, "ertex") == 0)
          data->token = VERTEX_token;
        break;
      default:
        data->token = unknown_token;
      }
    }

    if (data->token == unknown_token) {
      if (str[0] == '+' || str[0] == '-' || isdigit(str[0])) /* Try to guess if it's a float */
        data->token = FLOAT_token;
      else
        data->token = ID_token;
    }
  }
  else {
    data->token = unknown_token;
    parsing_error(data);
  }
}

static void parsing_eat_token(foug_stla_token_t token, foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  if (data->token == token)
    parsing_advance(data);
  else
    parsing_error(data);
}

static void parse_solidname_beg(foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  switch (data->token) {
  case ENDSOLID_token:
  case FACET_token:
  case ID_token:
    break;
  default:
    parsing_error(data);
  }
}

static void parse_solidname_end(foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  switch (data->token) {
  case SOLID_token:
  case ID_token:
    break;
  default:
    parsing_error(data);
  }
}

static void parse_beginsolid(foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  switch (data->token) {
  case SOLID_token: {
    parsing_eat_token(SOLID_token, data);
    parse_solidname_beg(data);
    if (parsing_can_continue(data) && data->geom != NULL && data->geom->begin_solid_func != NULL)
      data->geom->begin_solid_func(data->geom, current_token_as_identifier(data));
    if (data->token == ID_token)
      parsing_eat_token(ID_token, data);
    break;
  }
  default:
    parsing_error(data);
  } /* end switch */
}

static void parse_endsolid(foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  switch (data->token) {
  case ENDSOLID_token: {
    parsing_eat_token(ENDSOLID_token, data);
    parse_solidname_end(data);
    if (parsing_can_continue(data) && data->geom != NULL && data->geom->end_solid_func != NULL)
      data->geom->end_solid_func(data->geom, current_token_as_identifier(data));
    if (data->token == ID_token)
      parsing_eat_token(ID_token, data);
    break;
  }
  default:
    parsing_error(data);
  } /* end switch */
}

static void parse_xyz_coords(foug_stla_parse_data_t* data, foug_stl_coords_t* coords)
{
  if (!parsing_can_continue(data))
    return;

  switch (data->token) {
  case FLOAT_token: {
    if (get_current_token_as_real32(data, &coords->x) != 0)
      parsing_error(data);
    parsing_eat_token(FLOAT_token, data);
    if (get_current_token_as_real32(data, &coords->y) != 0)
      parsing_error(data);
    parsing_eat_token(FLOAT_token, data);
    if (get_current_token_as_real32(data, &coords->z) != 0)
      parsing_error(data);
    parsing_eat_token(FLOAT_token, data);
    break;
  }
  default:
    break;
  } /* end switch */
}

static void parse_facets(foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  switch (data->token) {
  case ENDSOLID_token:
    break;
  case FACET_token: {
    foug_stl_triangle_t facet;

    parsing_eat_token(FACET_token, data);
    parsing_eat_token(NORMAL_token, data);
    parse_xyz_coords(data, &facet.normal);

    parsing_eat_token(OUTER_token, data);
    parsing_eat_token(LOOP_token, data);

    parsing_eat_token(VERTEX_token, data);
    parse_xyz_coords(data, &facet.v1);
    parsing_eat_token(VERTEX_token, data);
    parse_xyz_coords(data, &facet.v2);
    parsing_eat_token(VERTEX_token, data);
    parse_xyz_coords(data, &facet.v3);

    parsing_eat_token(ENDLOOP_token, data);
    parsing_eat_token(ENDFACET_token, data);

    if (parsing_can_continue(data)
        && data->geom != NULL && data->geom->process_next_triangle_func != NULL)
    {
      data->geom->process_next_triangle_func(data->geom, &facet);
    }

    parse_facets(data);
    break;
  }
  default:
    parsing_error(data);
  } /* end switch */
}

static void parse_solid(foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  switch (data->token) {
  case SOLID_token:
    parse_beginsolid(data);
    parse_facets(data);
    parse_endsolid(data);
    break;
  default:
    parsing_error(data);
  }
}

static void parse_contents(foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  switch (data->token) {
  case SOLID_token:
    parse_solid(data);
    break;
  default:
    parsing_error(data);
  }
}

#define FOUG_STLA_READ_STRING_BUFFER_LEN    512

int foug_stla_read(foug_stla_geom_input_t* geom,
                   foug_transfer_t *trsf,
                   size_t data_size_hint)
{
  char fixed_buffer[FOUG_STLA_READ_STRING_BUFFER_LEN];
  foug_stla_parse_data_t parse_data;

  if (trsf == NULL)
    return FOUG_DATAX_NULL_TRANSFER_ERROR;
  if (trsf->buffer == NULL)
    return FOUG_DATAX_NULL_BUFFER_ERROR;
  if (trsf->buffer_size == 0)
    return FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;

  parse_data.token = unknown_token;
  parse_data.error = 0;

  parse_data.stream_iterator_cookie.task_control = &trsf->task_control;
  parse_data.stream_iterator_cookie.stream_data_size = data_size_hint;
  parse_data.stream_iterator_cookie.stream_offset = 0;
  parse_data.stream_iterator_cookie.is_stop_requested = 0;

  parse_data.stream_iterator.stream = &(trsf->stream);
  parse_data.stream_iterator.buffer = trsf->buffer;
  parse_data.stream_iterator.buffer_offset = 0;
  parse_data.stream_iterator.buffer_size = trsf->buffer_size;
  parse_data.stream_iterator.cookie = &parse_data.stream_iterator_cookie;
  parse_data.stream_iterator.stream_read_hook = foug_stream_fwd_iterator_stla_read_hook;
  foug_stream_fwd_iterator_init(&parse_data.stream_iterator);

  parse_data.string_buffer.data = fixed_buffer;
  parse_data.string_buffer.max_len = FOUG_STLA_READ_STRING_BUFFER_LEN;
  parse_data.string_buffer.len = 0;

  parse_data.geom = geom;

  parsing_advance(&parse_data);
  parse_contents(&parse_data);

  if (parse_data.error)
    return FOUG_STLA_READ_PARSE_ERROR;
  return parse_data.stream_iterator_cookie.is_stop_requested ? FOUG_DATAX_TASK_STOPPED_ERROR :
                                                               FOUG_DATAX_NO_ERROR;
}
