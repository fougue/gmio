/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include "stl_io.h"

#include "stl_error.h"
#include "internal/helper_stl_mesh_creator.h"
#include "internal/stl_rw_common.h"

#include "../gmio_core/error.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/helper_transfer.h"
#include "../gmio_core/internal/min_max.h"
#include "../gmio_core/internal/string_parse.h"
#include "../gmio_core/internal/string_utils.h"

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

/* Fixed maximum length of any string_buffer user in this source file */
enum { GMIO_STLA_READ_STRING_BUFFER_LEN = 1024 };

/* gmio_stream_fwd_iterator_stla_cookie */
typedef struct
{
    /* Copy of gmio_stla_read() corresponding argument */
    gmio_transfer_t* transfer;
    /* Cache for gmio_stream_size(&transfer->stream) */
    size_t stream_size;
    /* Offset (in bytes) from beginning of stream : current position */
    size_t stream_offset;
    /* Cache for gmio_transfer::func_is_stop_requested() */
    gmio_bool_t is_stop_requested;
} gmio_string_stream_fwd_iterator_cookie_t;

/* gmio_stla_token */
typedef enum
{
    ENDFACET_token = 0x0001,
    ENDLOOP_token = 0x0002,
    ENDSOLID_token = 0x0004,
    FACET_token = 0x0008,
    ID_token = 0x0010,
    FLOAT_token = ID_token,
    LOOP_token = 0x0020,
    NORMAL_token = 0x0040,
    OUTER_token = 0x0080,
    SOLID_token = 0x0100,
    VERTEX_token = 0x0200,
    empty_token = 0x0400,
    unknown_token = 0x0800
} gmio_stla_token_t;

/* gmio_stla_parse_data */
typedef struct
{
    gmio_stla_token_t token;
    gmio_bool_t error;
    gmio_string_stream_fwd_iterator_t stream_iterator;
    gmio_string_stream_fwd_iterator_cookie_t stream_iterator_cookie;
    gmio_string_buffer_t string_buffer;
    gmio_stl_mesh_creator_t* creator;
} gmio_stla_parse_data_t;

static void gmio_stream_fwd_iterator_stla_read_hook(
        void* cookie, const gmio_string_buffer_t* buffer)
{
    gmio_string_stream_fwd_iterator_cookie_t* tcookie =
            (gmio_string_stream_fwd_iterator_cookie_t*)(cookie);
    const gmio_transfer_t* trsf = tcookie != NULL ? tcookie->transfer : NULL;
    if (tcookie != NULL) {
        tcookie->stream_offset += buffer->len;
        tcookie->is_stop_requested = gmio_transfer_is_stop_requested(trsf);
        gmio_transfer_handle_progress(
                    trsf, tcookie->stream_offset, tcookie->stream_size);
    }
}

GMIO_INLINE gmio_bool_t parsing_can_continue(
        const gmio_stla_parse_data_t* data)
{
    if (!data->error && !data->stream_iterator_cookie.is_stop_requested)
        return GMIO_TRUE;
    return GMIO_FALSE;
}

GMIO_INLINE int get_current_token_as_float32(
        const gmio_stla_parse_data_t* data, gmio_float32_t* value_ptr)
{
    if (data->token == FLOAT_token)
        return gmio_get_float32(data->string_buffer.ptr, value_ptr);
    return -3;
}

GMIO_INLINE const char* token_to_string(gmio_stla_token_t token)
{
    switch (token) {
    case ENDFACET_token: return "endfacet";
    case ENDLOOP_token: return "endloop";
    case ENDSOLID_token: return "endsolid";
    case FACET_token: return "facet";
    case ID_token: return "ID";
    /*case FLOAT_token: return "FLOAT";*/
    case LOOP_token: return "loop";
    case NORMAL_token: return "normal";
    case OUTER_token: return "outer";
    case SOLID_token: return "solid";
    case VERTEX_token: return "vertex";
    case empty_token:
    case unknown_token: return "";
    }
    return "";
}

GMIO_INLINE void parsing_error_msg(
        gmio_stla_parse_data_t* data, const char* msg)
{
    fprintf(stderr,
            "\n"
            "gmio_stla_read() parsing error: %s\n"
            "                 current token: <%s>\n"
            "                 current token string: \"%s\"\n",
            msg,
            token_to_string(data->token),
            data->string_buffer.ptr);
    data->error = GMIO_TRUE;
    data->token = unknown_token;
}

GMIO_INLINE void parsing_error(gmio_stla_parse_data_t* data)
{
    parsing_error_msg(data, "unknown");
}

GMIO_INLINE void parsing_error_token_expected(
        gmio_stla_parse_data_t* data, gmio_stla_token_t token)
{
    char msg[256] = { 0 };
    sprintf(msg,
            "token <%s> expected, got <%s>",
            token_to_string(token),
            token_to_string(data->token));
    parsing_error_msg(data, msg);
}

static gmio_stla_token_t parsing_find_token(const char* word, size_t word_len)
{
    /* Get rid of ill-formed token */
    if (word_len == 0)
        return empty_token;

    /* Try to find non "endXxx" token */
    if (word_len >= 4) {
        switch (word[0]) {
        case 'f':
        case 'F':
            if (gmio_stricmp(word + 1, "acet") == 0)
                return FACET_token;
            break;
        case 'l':
        case 'L':
            if (gmio_stricmp(word + 1, "oop") == 0)
                return LOOP_token;
            break;
        case 'n':
        case 'N':
            if (gmio_stricmp(word + 1, "ormal") == 0)
                return NORMAL_token;
            break;
        case 'o':
        case 'O':
            if (gmio_stricmp(word + 1, "uter") == 0)
                return OUTER_token;
            break;
        case 's':
        case 'S':
            if (gmio_stricmp(word + 1, "olid") == 0)
                return SOLID_token;
            break;
        case 'v':
        case 'V':
            if (gmio_stricmp(word + 1, "ertex") == 0)
                return VERTEX_token;
            break;
        default:
            break;
        }
    }

    /* Might be "end..." token */
    if (word_len >= 7 && gmio_istarts_with(word, "end") == GMIO_TRUE) {
        switch (word[3]) {
        case 'f':
        case 'F':
            if (gmio_stricmp(word + 4, "acet") == 0)
                return ENDFACET_token;
            break;
        case 'l':
        case 'L':
            if (gmio_stricmp(word + 4, "oop") == 0)
                return ENDLOOP_token;
            break;
        case 's':
        case 'S':
            if (gmio_stricmp(word + 4, "olid") == 0)
                return ENDSOLID_token;
            break;
        default:
            break;
        }
    }

    return ID_token;
}

GMIO_INLINE gmio_stla_token_t parsing_find_token_from_buff(
        const gmio_string_buffer_t* str_buffer)
{
    return parsing_find_token(str_buffer->ptr, str_buffer->len);
}

GMIO_INLINE void parsing_advance(gmio_stla_parse_data_t* data)
{
    if (!parsing_can_continue(data))
        return;

    data->string_buffer.len = 0;
    if (gmio_eat_word(&data->stream_iterator, &data->string_buffer) == 0)
        data->token = parsing_find_token_from_buff(&data->string_buffer);
    else
        data->token = unknown_token;

    if (data->token == unknown_token)
        parsing_error(data);
}

GMIO_INLINE void parsing_eat_token(
        gmio_stla_token_t token, gmio_stla_parse_data_t* data)
{
    if (!parsing_can_continue(data))
        return;

    if (data->token == token)
        parsing_advance(data);
    else
        parsing_error_token_expected(data, token);
}

static void parse_eat_until_token(
        gmio_stla_parse_data_t* data,
        int end_tokens)
{
    if (!parsing_can_continue(data))
        return;
    if ((data->token & end_tokens) == 0) {
        gmio_string_stream_fwd_iterator_t* stream_it = &data->stream_iterator;
        gmio_string_buffer_t* string_buf = &data->string_buffer;
        int end_token_found = 0;

        do {
            const size_t previous_buff_len = string_buf->len;
            int res_eat_word = 0; /* Result of gmio_eat_word() */
            const char* next_word = NULL; /* Pointer on next word string */
            size_t next_word_len = 0; /* Length of next word string */

            gmio_copy_spaces(stream_it, string_buf);
            /* Next word */
            next_word = string_buf->ptr + string_buf->len;
            res_eat_word = gmio_eat_word(stream_it, string_buf);
            next_word_len =
                    (string_buf->ptr + string_buf->len) - next_word;
            /* Qualify token */
            data->token =
                    res_eat_word == 0 ?
                        parsing_find_token(next_word, next_word_len) :
                        unknown_token;
            /* End token found ? */
            end_token_found = data->token & end_tokens;
            /* True ?
             * trim string_buf so it contains only contents before end token */
            if (end_token_found) {
                string_buf->len = previous_buff_len;
                string_buf->ptr[previous_buff_len] = 0;
            }
        } while (!end_token_found && string_buf->len < string_buf->max_len);

        if (!end_token_found) {
            parsing_error_msg(
                        data,
                        "end token not found in parse_eat_until_token()");
        }
    }
}

static void parse_solidname_beg(gmio_stla_parse_data_t* data)
{
    if (!parsing_can_continue(data))
        return;

    if (data->token == FACET_token || data->token == ENDSOLID_token) {
        gmio_string_buffer_clear(&data->string_buffer);
    }
    else {
        /* Solid name can be made of multiple words */
        parse_eat_until_token(data, FACET_token | ENDSOLID_token);
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
        parsing_error_msg(data, "unexpected token for 'endsolid <name>'");
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
        if (parsing_can_continue(data)) {
            gmio_stl_mesh_creator_ascii_begin_solid(
                        data->creator,
                        data->stream_iterator_cookie.stream_size,
                        data->string_buffer.ptr);
        }
        if (data->token == ID_token)
            parsing_eat_token(ID_token, data);
        break;
    }
    default:
        parsing_error_token_expected(data, SOLID_token);
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
        if (parsing_can_continue(data))
            gmio_stl_mesh_creator_end_solid(data->creator);
        if (data->token == ID_token)
            parsing_eat_token(ID_token, data);
        break;
    }
    default:
        parsing_error_token_expected(data, ENDSOLID_token);
    } /* end switch */
}

static void parse_xyz_coords(
        gmio_stla_parse_data_t* data, gmio_stl_coords_t* coords)
{
    if (!parsing_can_continue(data))
        return;

    switch (data->token) {
    case FLOAT_token: {
        if (get_current_token_as_float32(data, &coords->x) != 0)
            parsing_error_msg(data, "failed to get X coord");
        parsing_eat_token(FLOAT_token, data);
        if (get_current_token_as_float32(data, &coords->y) != 0)
            parsing_error_msg(data, "failed to get Y coord");
        parsing_eat_token(FLOAT_token, data);
        if (get_current_token_as_float32(data, &coords->z) != 0)
            parsing_error_msg(data, "failed to get Z coord");
        parsing_eat_token(FLOAT_token, data);
        break;
    }
    default:
        break;
    } /* end switch */
}

static void parse_facet(
        gmio_stla_parse_data_t* data, gmio_stl_triangle_t* facet)
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
    uint32_t i_facet = 0;
    gmio_stl_triangle_t facet;

    facet.attribute_byte_count = 0;
    while (data->token == FACET_token && parsing_can_continue(data)) {
        parse_facet(data, &facet);
        gmio_stl_mesh_creator_add_triangle(data->creator, i_facet, &facet);
        ++i_facet;
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

int gmio_stla_read(gmio_transfer_t* trsf, gmio_stl_mesh_creator_t* creator)
{
    char fixed_buffer[GMIO_STLA_READ_STRING_BUFFER_LEN];
    gmio_stla_parse_data_t parse_data;

    { /* Check validity of input parameters */
        int error = GMIO_ERROR_OK;
        if (!gmio_check_transfer(&error, trsf))
            return error;
    }

    parse_data.token = unknown_token;
    parse_data.error = GMIO_FALSE;

    parse_data.stream_iterator_cookie.transfer = trsf;
    parse_data.stream_iterator_cookie.stream_offset = 0;
    parse_data.stream_iterator_cookie.stream_size =
            gmio_stream_size(&trsf->stream);
    parse_data.stream_iterator_cookie.is_stop_requested = GMIO_FALSE;

    parse_data.stream_iterator.stream = &trsf->stream;
    parse_data.stream_iterator.buffer.ptr = trsf->buffer.ptr;
    parse_data.stream_iterator.buffer.max_len = trsf->buffer.size;
    parse_data.stream_iterator.cookie = &parse_data.stream_iterator_cookie;
    parse_data.stream_iterator.stream_read_hook =
            gmio_stream_fwd_iterator_stla_read_hook;
    gmio_string_stream_fwd_iterator_init(&parse_data.stream_iterator);

    parse_data.string_buffer.ptr = &fixed_buffer[0];
    parse_data.string_buffer.len = 0;
    parse_data.string_buffer.max_len = GMIO_STLA_READ_STRING_BUFFER_LEN;

    parse_data.creator = creator;

    parsing_advance(&parse_data);
    parse_solid(&parse_data);

    if (parse_data.error)
        return GMIO_STL_ERROR_PARSING;
    if (parse_data.stream_iterator_cookie.is_stop_requested)
        return GMIO_ERROR_TRANSFER_STOPPED;
    return GMIO_ERROR_OK;
}
