#include "stla_read.h"

#include "../internal/ascii_parse.h"
#include "../internal/libstl/stl_rw_common.h"
#include "../error.h"
#include "stl_error.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

/*
 *
 * STL ASCII grammar:
 *
 *  \code

    CONTENTS  -> SOLID
    CONTENTS  -> SOLID  SOLID

    SOLID     -> BEG_SOLID  FACETS  END_SOLID
    BEG_SOLID -> solid  SOLID_NAME
    END_SOLID -> endsolid SOLID_NAME

    SOLID_NAME ->
    SOLID_NAME -> [id]  (Note: [id] == facet is forbidden)

    FACETS ->
    FACETS -> F
    FACETS -> FF
    F      -> facet N outer loop V V V endloop endfacet

    V   -> vertex XYZ
    N   -> normal XYZ
    XYZ -> [float] [float] [float]

 *  \endcode
 *
 *  Nullable, FIRST and FOLLOW:
 *  \code
                | Nullable |     FIRST    |           FOLLOW
      ----------+----------+--------------+-----------------------------
       CONTENTS |    N           solid
          SOLID |    N           solid                 solid
      BEG_SOLID |    N           solid                 facet
      END_SOLID |    N          endsolid               solid
     SOLID_NAME |    Y           [id]           facet, endsolid, solid
         FACETS |    Y           facet
              F |    N           facet             facet, endsolid
              N |    N           normal                 outer
              V |    N           vertex             vertex, endloop
            XYZ |    N           [float]        outer, vertex, endloop

 *  \endcode
 *
 */

/* foug_stream_fwd_iterator_stla_cookie */
typedef struct
{
  foug_task_control_t* task_control;
  size_t               stream_data_size;
  size_t               stream_offset;
  foug_bool_t          is_stop_requested;
} _internal_foug_fwd_iterator_cookie_t;

/* foug_stla_token */
typedef enum
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
  empty_token,
  unknown_token
} foug_stla_token_t;

/* foug_stla_parse_data */
typedef struct
{
  foug_stla_token_t token;
  foug_bool_t       error;
  foug_ascii_stream_fwd_iterator_t     stream_iterator;
  _internal_foug_fwd_iterator_cookie_t stream_iterator_cookie;
  foug_ascii_string_buffer_t           string_buffer;
  foug_stl_geom_creator_t*             creator;
} foug_stla_parse_data_t;

static void foug_stream_fwd_iterator_stla_read_hook(void* cookie,
                                                    const foug_ascii_string_buffer_t* buffer)
{
  _internal_foug_fwd_iterator_cookie_t* tcookie = (_internal_foug_fwd_iterator_cookie_t*)(cookie);
  if (tcookie != NULL) {
    const uint8_t progress_pc = foug_percentage(0, tcookie->stream_data_size, tcookie->stream_offset);
    tcookie->is_stop_requested = !foug_task_control_handle_progress(tcookie->task_control, progress_pc);
    tcookie->stream_offset += buffer->len;
  }
}

FOUG_INLINE static foug_bool_t parsing_can_continue(const foug_stla_parse_data_t* data)
{
  if (data->error || data->stream_iterator_cookie.is_stop_requested)
    return 0;
  return 1;
}

FOUG_INLINE static const char* current_token_as_identifier(const foug_stla_parse_data_t* data)
{
  return data->token == ID_token ? data->string_buffer.ptr : "";
}

FOUG_INLINE static int get_current_token_as_real32(const foug_stla_parse_data_t* data,
                                                   foug_real32_t* value_ptr)
{
  if (data->token == FLOAT_token)
    return foug_get_real32(data->string_buffer.ptr, value_ptr);
  return -3;
}

FOUG_INLINE static void parsing_error(foug_stla_parse_data_t* data)
{
  data->error = 1;
  data->token = unknown_token;
  printf("parsing_error, token: %s\n", data->string_buffer.ptr);
}

static foug_stla_token_t parsing_find_token(const foug_ascii_string_buffer_t* str_buffer)
{
  const char* word = str_buffer->ptr;
  const size_t word_len = str_buffer->len;

  /* Get rid of ill-formed token */
  if (word_len == 0)
    return empty_token;

  /* Try to guess if it's a float */
  if (word[0] == '+' || word[0] == '-' || isdigit(word[0]))
    return FLOAT_token;

  /* Try to find non "endXxx" token */
  if (word_len >= 4) {
    switch (word[0]) {
    case 'f':
      if (strcmp(word + 1, "acet") == 0)
        return FACET_token;
      break;
    case 'l':
      if (strcmp(word + 1, "oop") == 0)
        return LOOP_token;
      break;
    case 'n':
      if (strcmp(word + 1, "ormal") == 0)
        return NORMAL_token;
      break;
    case 'o':
      if (strcmp(word + 1, "uter") == 0)
        return OUTER_token;
      break;
    case 's':
      if (strcmp(word + 1, "olid") == 0)
        return SOLID_token;
      break;
    case 'v':
      if (strcmp(word + 1, "ertex") == 0)
        return VERTEX_token;
      break;
    default:
      break;
    }
  }

  /* Might be "end..." token */
  if (word_len >= 7 && strncmp(word, "end", 3) == 0) {
    switch (word[3]) {
    case 'f':
      if (strcmp(word + 4, "acet") == 0)
        return ENDFACET_token;
      break;
    case 'l':
      if (strcmp(word + 4, "oop") == 0)
        return ENDLOOP_token;
      break;
    case 's':
      if (strcmp(word + 4, "olid") == 0)
        return ENDSOLID_token;
      break;
    default:
      break;
    }
  }

  return ID_token;
}

static void parsing_advance(foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  if (foug_eat_word(&data->stream_iterator, &data->string_buffer) == 0)
    data->token = parsing_find_token(&data->string_buffer);
  else
    data->token = unknown_token;

  if (data->token == unknown_token)
    parsing_error(data);
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
  case empty_token:
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
    if (parsing_can_continue(data)
        && data->creator != NULL
        && data->creator->ascii_begin_solid_func != NULL)
    {
      data->creator->ascii_begin_solid_func(data->creator->cookie,
                                            current_token_as_identifier(data));
    }
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
    if (parsing_can_continue(data)
        && data->creator != NULL
        && data->creator->end_solid_func != NULL)
    {
      data->creator->end_solid_func(data->creator->cookie/*, current_token_as_identifier(data)*/);
    }
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

static void parse_facet(foug_stla_parse_data_t* data, foug_stl_triangle_t* facet)
{
  parsing_eat_token(FACET_token, data);
  parsing_eat_token(NORMAL_token, data);
  parse_xyz_coords(data, &facet->normal);

  parsing_eat_token(OUTER_token, data);
  parsing_eat_token(LOOP_token, data);

  parsing_eat_token(VERTEX_token, data);
  parse_xyz_coords(data, &facet->v1);
  parsing_eat_token(VERTEX_token, data);
  parse_xyz_coords(data, &facet->v2);
  parsing_eat_token(VERTEX_token, data);
  parse_xyz_coords(data, &facet->v3);

  parsing_eat_token(ENDLOOP_token, data);
  parsing_eat_token(ENDFACET_token, data);
}

static void parse_facets(foug_stla_parse_data_t* data)
{
  uint32_t i_facet_offset = 0;
  foug_stl_triangle_t facet;

  while (data->token == FACET_token && parsing_can_continue(data)) {
    parse_facet(data, &facet);
    if (data->creator != NULL && data->creator->add_triangle_func != NULL)
      data->creator->add_triangle_func(data->creator->cookie, i_facet_offset, &facet);
    ++i_facet_offset;
  }
}

static void parse_solid(foug_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  if (data->token == SOLID_token) {
    parse_beginsolid(data);
    parse_facets(data);
    parse_endsolid(data);
  }
  else {
    parsing_error(data);
  }
}

#define FOUG_STLA_READ_STRING_BUFFER_LEN    512

int foug_stla_read(foug_stl_geom_creator_t *creator,
                   foug_transfer_t *trsf,
                   size_t data_size_hint)
{
  char fixed_buffer[FOUG_STLA_READ_STRING_BUFFER_LEN];
  foug_stla_parse_data_t parse_data;

  { /* Check validity of input parameters */
    int error = FOUG_DATAX_NO_ERROR;
    if (!foug_check_transfer(&error, trsf))
      return error;
  }

  parse_data.token = unknown_token;
  parse_data.error = 0;

  parse_data.stream_iterator_cookie.task_control = &trsf->task_control;
  parse_data.stream_iterator_cookie.stream_data_size = data_size_hint;
  parse_data.stream_iterator_cookie.stream_offset = 0;
  parse_data.stream_iterator_cookie.is_stop_requested = 0;

  parse_data.stream_iterator.stream = &trsf->stream;
  parse_data.stream_iterator.buffer.ptr = trsf->buffer;
  parse_data.stream_iterator.buffer.max_len = trsf->buffer_size;
  parse_data.stream_iterator.cookie = &parse_data.stream_iterator_cookie;
  parse_data.stream_iterator.stream_read_hook = foug_stream_fwd_iterator_stla_read_hook;
  foug_ascii_stream_fwd_iterator_init(&parse_data.stream_iterator);

  parse_data.string_buffer.ptr = &fixed_buffer[0];
  parse_data.string_buffer.len = 0;
  parse_data.string_buffer.max_len = FOUG_STLA_READ_STRING_BUFFER_LEN;

  parse_data.creator = creator;

  parsing_advance(&parse_data);
  parse_solid(&parse_data);

  if (parse_data.error)
    return FOUG_STLA_READ_PARSE_ERROR;
  return parse_data.stream_iterator_cookie.is_stop_requested ? FOUG_DATAX_TASK_STOPPED_ERROR :
                                                               FOUG_DATAX_NO_ERROR;
}
