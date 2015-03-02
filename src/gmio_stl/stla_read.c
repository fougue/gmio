/****************************************************************************
**
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
**
****************************************************************************/

#include "stl_io.h"

#include "stl_error.h"
#include "internal/stl_rw_common.h"

#include "../gmio_core/error.h"
#include "../gmio_core/internal/ascii_parse.h"

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

/* gmio_stream_fwd_iterator_stla_cookie */
typedef struct
{
  gmio_task_control_t* task_control;
  size_t               stream_offset;
  gmio_bool_t          is_stop_requested;
} _internal_gmio_fwd_iterator_cookie_t;

/* gmio_stla_token */
typedef enum
{
  ENDFACET_token,
  ENDLOOP_token,
  ENDSOLID_token,
  FACET_token,
  ID_token,
  FLOAT_token = ID_token,
  LOOP_token,
  NORMAL_token,
  OUTER_token,
  SOLID_token,
  VERTEX_token,
  empty_token,
  unknown_token
} gmio_stla_token_t;

/* gmio_stla_parse_data */
typedef struct
{
  gmio_stla_token_t token;
  gmio_bool_t       error;
  gmio_ascii_stream_fwd_iterator_t     stream_iterator;
  _internal_gmio_fwd_iterator_cookie_t stream_iterator_cookie;
  gmio_ascii_string_buffer_t           string_buffer;
  gmio_stl_mesh_creator_t*             creator;
} gmio_stla_parse_data_t;

static void gmio_stream_fwd_iterator_stla_read_hook(void* cookie,
                                                    const gmio_ascii_string_buffer_t* buffer)
{
  _internal_gmio_fwd_iterator_cookie_t* tcookie = (_internal_gmio_fwd_iterator_cookie_t*)(cookie);
  const gmio_task_control_t* ctrl = tcookie != NULL ? tcookie->task_control : NULL;
  if (ctrl != NULL)
    tcookie->is_stop_requested = gmio_task_control_is_stop_requested(ctrl);
  if (tcookie != NULL)
    tcookie->stream_offset += buffer->len;
}

GMIO_INLINE static gmio_bool_t parsing_can_continue(const gmio_stla_parse_data_t* data)
{
  if (!data->error && !data->stream_iterator_cookie.is_stop_requested)
    return GMIO_TRUE;
  return GMIO_FALSE;
}

GMIO_INLINE static const char* current_token_as_identifier(const gmio_stla_parse_data_t* data)
{
  return data->token == ID_token ? data->string_buffer.ptr : "";
}

GMIO_INLINE static int get_current_token_as_real32(const gmio_stla_parse_data_t* data,
                                                   gmio_real32_t* value_ptr)
{
  if (data->token == FLOAT_token)
    return gmio_get_real32(data->string_buffer.ptr, value_ptr);
  return -3;
}

GMIO_INLINE static void parsing_error(gmio_stla_parse_data_t* data)
{
  data->error = GMIO_TRUE;
  data->token = unknown_token;
  fprintf(stderr,
          "\n[gmio_stla_read()] parsing_error, token: \"%s\"\n",
          data->string_buffer.ptr);
}

static gmio_stla_token_t parsing_find_token(const gmio_ascii_string_buffer_t* str_buffer)
{
  const char* word = str_buffer->ptr;
  const size_t word_len = str_buffer->len;

  /* Get rid of ill-formed token */
  if (word_len == 0)
    return empty_token;

  /* Try to find non "endXxx" token */
  if (word_len >= 4) {
    switch (word[0]) {
    case 'f':
    case 'F':
      if (strcmp(word + 1, "acet") == 0)
        return FACET_token;
      break;
    case 'l':
    case 'L':
      if (strcmp(word + 1, "oop") == 0)
        return LOOP_token;
      break;
    case 'n':
    case 'N':
      if (strcmp(word + 1, "ormal") == 0)
        return NORMAL_token;
      break;
    case 'o':
    case 'O':
      if (strcmp(word + 1, "uter") == 0)
        return OUTER_token;
      break;
    case 's':
    case 'S':
      if (strcmp(word + 1, "olid") == 0)
        return SOLID_token;
      break;
    case 'v':
    case 'V':
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
    case 'F':
      if (strcmp(word + 4, "acet") == 0)
        return ENDFACET_token;
      break;
    case 'l':
    case 'L':
      if (strcmp(word + 4, "oop") == 0)
        return ENDLOOP_token;
      break;
    case 's':
    case 'S':
      if (strcmp(word + 4, "olid") == 0)
        return ENDSOLID_token;
      break;
    default:
      break;
    }
  }

  return ID_token;
}

static void parsing_advance(gmio_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  if (gmio_eat_word(&data->stream_iterator, &data->string_buffer) == 0)
    data->token = parsing_find_token(&data->string_buffer);
  else
    data->token = unknown_token;

  if (data->token == unknown_token)
    parsing_error(data);
}

static void parsing_eat_token(gmio_stla_token_t token, gmio_stla_parse_data_t* data)
{
  if (!parsing_can_continue(data))
    return;

  if (data->token == token)
    parsing_advance(data);
  else
    parsing_error(data);
}

static void parse_solidname_beg(gmio_stla_parse_data_t* data)
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

static void parse_solidname_end(gmio_stla_parse_data_t* data)
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

static void parse_beginsolid(gmio_stla_parse_data_t* data)
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

static void parse_endsolid(gmio_stla_parse_data_t* data)
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

static void parse_xyz_coords(gmio_stla_parse_data_t* data, gmio_stl_coords_t* coords)
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

static void parse_facet(gmio_stla_parse_data_t* data, gmio_stl_triangle_t* facet)
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

static void parse_facets(gmio_stla_parse_data_t* data)
{
  uint32_t i_facet_offset = 0;
  gmio_stl_triangle_t facet;

  while (data->token == FACET_token && parsing_can_continue(data)) {
    parse_facet(data, &facet);
    if (data->creator != NULL && data->creator->add_triangle_func != NULL)
      data->creator->add_triangle_func(data->creator->cookie, i_facet_offset, &facet);
    ++i_facet_offset;
  }
}

static void parse_solid(gmio_stla_parse_data_t* data)
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

enum { GMIO_STLA_READ_STRING_BUFFER_LEN = 512 };

int gmio_stla_read(gmio_stl_mesh_creator_t* creator,
                   gmio_transfer_t* trsf,
                   const gmio_stla_read_options_t* options)
{
  char fixed_buffer[GMIO_STLA_READ_STRING_BUFFER_LEN];
  gmio_stla_parse_data_t parse_data;
  GMIO_UNUSED(options);

  { /* Check validity of input parameters */
    int error = GMIO_NO_ERROR;
    if (!gmio_check_transfer(&error, trsf))
      return error;
  }

  parse_data.token = unknown_token;
  parse_data.error = GMIO_FALSE;

  parse_data.stream_iterator_cookie.task_control = &trsf->task_control;
  parse_data.stream_iterator_cookie.stream_offset = 0;
  parse_data.stream_iterator_cookie.is_stop_requested = GMIO_FALSE;

  parse_data.stream_iterator.stream = &trsf->stream;
  parse_data.stream_iterator.buffer.ptr = trsf->buffer;
  parse_data.stream_iterator.buffer.max_len = trsf->buffer_size;
  parse_data.stream_iterator.cookie = &parse_data.stream_iterator_cookie;
  parse_data.stream_iterator.stream_read_hook = gmio_stream_fwd_iterator_stla_read_hook;
  gmio_ascii_stream_fwd_iterator_init(&parse_data.stream_iterator);

  parse_data.string_buffer.ptr = &fixed_buffer[0];
  parse_data.string_buffer.len = 0;
  parse_data.string_buffer.max_len = GMIO_STLA_READ_STRING_BUFFER_LEN;

  parse_data.creator = creator;

  parsing_advance(&parse_data);
  parse_solid(&parse_data);

  if (parse_data.error)
    return GMIO_STLA_READ_PARSE_ERROR;
  return parse_data.stream_iterator_cookie.is_stop_requested ? GMIO_TASK_STOPPED_ERROR :
                                                               GMIO_NO_ERROR;
}
