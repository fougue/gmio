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
#include "internal/stl_funptr_typedefs.h"
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

/* Fixed maximum length of any string user in this source file */
enum { GMIO_STLA_READ_STRING_MAX_LEN = 1024 };

/* gmio_stla_token */
enum gmio_stla_token
{
    null_token = 0,
    ENDFACET_token,
    ENDLOOP_token,
    ENDSOLID_token,
    FACET_token,
    LOOP_token,
    NORMAL_token,
    OUTER_token,
    SOLID_token,
    VERTEX_token,
    ID_token,
    FLOAT_token = ID_token,
    empty_token,
    unknown_token
};
typedef enum gmio_stla_token gmio_stla_token_t;

static const char gmio_stla_tokstr_ENDFACET[] = "endfacet";
static const char gmio_stla_tokstr_ENDLOOP[] = "endloop";
static const char gmio_stla_tokstr_ENDSOLID[] = "endsolid";
static const char gmio_stla_tokstr_FACET[] = "facet";
static const char gmio_stla_tokstr_LOOP[] = "loop";
static const char gmio_stla_tokstr_NORMAL[] = "normal";
static const char gmio_stla_tokstr_OUTER[] = "outer";
static const char gmio_stla_tokstr_SOLID[] = "solid";
static const char gmio_stla_tokstr_VERTEX[] = "vertex";

static const gmio_const_string_t gmio_stla_tokcstr[] = {
    {0},
    GMIO_CONST_STRING_FROM_ARRAY(gmio_stla_tokstr_ENDFACET),
    GMIO_CONST_STRING_FROM_ARRAY(gmio_stla_tokstr_ENDLOOP),
    GMIO_CONST_STRING_FROM_ARRAY(gmio_stla_tokstr_ENDSOLID),
    GMIO_CONST_STRING_FROM_ARRAY(gmio_stla_tokstr_FACET),
    GMIO_CONST_STRING_FROM_ARRAY(gmio_stla_tokstr_LOOP),
    GMIO_CONST_STRING_FROM_ARRAY(gmio_stla_tokstr_NORMAL),
    GMIO_CONST_STRING_FROM_ARRAY(gmio_stla_tokstr_OUTER),
    GMIO_CONST_STRING_FROM_ARRAY(gmio_stla_tokstr_SOLID),
    GMIO_CONST_STRING_FROM_ARRAY(gmio_stla_tokstr_VERTEX),
    { "ID", 2 }, /* ID_token */
    { "",   0 }, /* empty_token */
    { "?",  1 }  /* unknown_token */
};

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

/* gmio_stla_parse_data */
typedef struct
{
    gmio_stla_token_t token;
    gmio_bool_t error;
    gmio_string_stream_fwd_iterator_t stream_iterator;
    gmio_string_stream_fwd_iterator_cookie_t stream_iterator_cookie;
    gmio_string_t string_buffer;
    gmio_stl_mesh_creator_t* creator;
} gmio_stla_parse_data_t;

static void gmio_stream_fwd_iterator_stla_read_hook(
        void* cookie, const gmio_string_t* buffer)
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
    return !data->error && !data->stream_iterator_cookie.is_stop_requested;
}

GMIO_INLINE const char* token_to_string(gmio_stla_token_t token)
{
    return gmio_stla_tokcstr[token].ptr;
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
    char msg[256] = {0};
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
            if (gmio_ascii_stricmp(word + 1, "acet") == 0)
                return FACET_token;
            break;
        case 'l':
        case 'L':
            if (gmio_ascii_stricmp(word + 1, "oop") == 0)
                return LOOP_token;
            break;
        case 'n':
        case 'N':
            if (gmio_ascii_stricmp(word + 1, "ormal") == 0)
                return NORMAL_token;
            break;
        case 'o':
        case 'O':
            if (gmio_ascii_stricmp(word + 1, "uter") == 0)
                return OUTER_token;
            break;
        case 's':
        case 'S':
            if (gmio_ascii_stricmp(word + 1, "olid") == 0)
                return SOLID_token;
            break;
        case 'v':
        case 'V':
            if (gmio_ascii_stricmp(word + 1, "ertex") == 0)
                return VERTEX_token;
            break;
        default:
            break;
        }
    }

    /* Might be "end..." token */
    if (word_len >= 7 && gmio_ascii_istarts_with(word, "end")) {
        switch (word[3]) {
        case 'f':
        case 'F':
            if (gmio_ascii_stricmp(word + 4, "acet") == 0)
                return ENDFACET_token;
            break;
        case 'l':
        case 'L':
            if (gmio_ascii_stricmp(word + 4, "oop") == 0)
                return ENDLOOP_token;
            break;
        case 's':
        case 'S':
            if (gmio_ascii_stricmp(word + 4, "olid") == 0)
                return ENDSOLID_token;
            break;
        default:
            break;
        }
    }

    return ID_token;
}

GMIO_INLINE gmio_stla_token_t parsing_find_token_from_buff(
        const gmio_string_t* str_buffer)
{
    return parsing_find_token(str_buffer->ptr, str_buffer->len);
}

static gmio_bool_t parsing_eat_next_token(
        gmio_stla_token_t next_token, gmio_stla_parse_data_t* data)
{
    gmio_string_t* data_strbuff = &data->string_buffer;
    gmio_eat_word_error_t eat_error;

    data_strbuff->len = 0;
    eat_error = gmio_eat_word(&data->stream_iterator, data_strbuff);
    if (eat_error == GMIO_EAT_WORD_ERROR_OK) {
        const char* next_token_str = token_to_string(next_token);
        if (gmio_ascii_stricmp(data_strbuff->ptr, next_token_str) == 0) {
            data->token = next_token;
            return GMIO_TRUE;
        }
        data->token = unknown_token;
        if (next_token == unknown_token)
            return GMIO_TRUE;
        parsing_error_token_expected(data, next_token);
        return GMIO_FALSE;
    }
    else {
        parsing_error_msg(
                    data, "failure to get next word with gmio_eat_word()");
        return GMIO_FALSE;
    }
}

GMIO_INLINE gmio_bool_t token_match_candidate(
        gmio_stla_token_t token, const gmio_stla_token_t* candidates)
{
    gmio_bool_t found = GMIO_FALSE;
    size_t i;
    for (i = 0; !found && candidates[i] != null_token; ++i)
        found = token == candidates[i];
    return found;
}

static gmio_bool_t parse_eat_until_token(
        gmio_stla_parse_data_t* data,
        const gmio_stla_token_t* end_tokens)
{
    if (!token_match_candidate(data->token, end_tokens)) {
        gmio_string_stream_fwd_iterator_t* stream_it = &data->stream_iterator;
        gmio_string_t* string_buf = &data->string_buffer;
        gmio_bool_t end_token_found = GMIO_FALSE;

        do {
            const size_t previous_buff_len = string_buf->len;
            gmio_eat_word_error_t eat_word_err = 0;
            const char* next_word = NULL; /* Pointer on next word string */
            size_t next_word_len = 0; /* Length of next word string */

            gmio_copy_spaces(stream_it, string_buf);
            /* Next word */
            next_word = string_buf->ptr + string_buf->len;
            eat_word_err = gmio_eat_word(stream_it, string_buf);
            next_word_len =
                    (string_buf->ptr + string_buf->len) - next_word;
            /* Qualify token */
            data->token =
                    eat_word_err == GMIO_EAT_WORD_ERROR_OK ?
                        parsing_find_token(next_word, next_word_len) :
                        unknown_token;
            /* End token found ? */
            end_token_found = token_match_candidate(data->token, end_tokens);
            /* True ?
             * trim string_buf so it contains only contents before end token */
            if (end_token_found) {
                string_buf->len = previous_buff_len;
                string_buf->ptr[previous_buff_len] = 0;
            }
        } while (!end_token_found && string_buf->len < string_buf->max_len);

        if (!end_token_found) {
            parsing_error_msg(
                        data, "end token not found in parse_eat_until_token()");
        }
        return end_token_found;
    }
    return GMIO_TRUE;
}

static gmio_bool_t parse_solidname_beg(gmio_stla_parse_data_t* data)
{
    if (parsing_eat_next_token(unknown_token, data)) {
        data->token = parsing_find_token_from_buff(&data->string_buffer);
        if (data->token == FACET_token || data->token == ENDSOLID_token) {
            gmio_string_clear(&data->string_buffer);
            return GMIO_TRUE;
        }
        else {
            /* Solid name can be made of multiple words */
            const gmio_stla_token_t end_tokens[] = {
                FACET_token, ENDSOLID_token, null_token };
            return parse_eat_until_token(data, end_tokens);
        }
    }
    return GMIO_FALSE;
}

static gmio_bool_t parse_solidname_end(gmio_stla_parse_data_t* data)
{
    GMIO_UNUSED(data);
    /* TODO: parse according to retrieved solid name */
    return GMIO_TRUE;
}

static gmio_bool_t parse_beginsolid(gmio_stla_parse_data_t* data)
{
    if (parsing_eat_next_token(SOLID_token, data)) {
        if (parse_solidname_beg(data)) {
            gmio_stl_mesh_creator_ascii_begin_solid(
                        data->creator,
                        data->stream_iterator_cookie.stream_size,
                        data->string_buffer.ptr);
            return GMIO_TRUE;
        }
    }
    return GMIO_FALSE;
}

static gmio_bool_t parse_endsolid(gmio_stla_parse_data_t* data)
{
    if (data->token == ENDSOLID_token
            || parsing_eat_next_token(ENDSOLID_token, data))
    {
        parse_solidname_end(data);
        gmio_stl_mesh_creator_end_solid(data->creator);
        return GMIO_TRUE;
    }
    return GMIO_FALSE;
}

static void parse_xyz_coords(
        gmio_stla_parse_data_t* data, gmio_stl_coords_t* coords)
{
    const char* strbuff_ptr = data->string_buffer.ptr;
    parsing_eat_next_token(unknown_token, data);
    gmio_get_float32(strbuff_ptr, &coords->x);
    parsing_eat_next_token(unknown_token, data);
    gmio_get_float32(strbuff_ptr, &coords->y);
    parsing_eat_next_token(unknown_token, data);
    gmio_get_float32(strbuff_ptr, &coords->z);
}

static void parse_facet(
        gmio_stla_parse_data_t* data, gmio_stl_triangle_t* facet)
{
    if (data->token != FACET_token)
        parsing_eat_next_token(FACET_token, data);

    parsing_eat_next_token(NORMAL_token, data);
    parse_xyz_coords(data, &facet->normal);

    parsing_eat_next_token(OUTER_token, data);
    parsing_eat_next_token(LOOP_token, data);

    parsing_eat_next_token(VERTEX_token, data);
    parse_xyz_coords(data, &facet->v1);
    parsing_eat_next_token(VERTEX_token, data);
    parse_xyz_coords(data, &facet->v2);
    parsing_eat_next_token(VERTEX_token, data);
    parse_xyz_coords(data, &facet->v3);

    parsing_eat_next_token(ENDLOOP_token, data);
    parsing_eat_next_token(ENDFACET_token, data);
}

static void parse_facets(gmio_stla_parse_data_t* data)
{
    const gmio_stl_mesh_creator_func_add_triangle_t func_add_triangle =
            data->creator->func_add_triangle;
    void* creator_cookie = data->creator->cookie;
    uint32_t i_facet = 0;
    gmio_stl_triangle_t facet;

    facet.attribute_byte_count = 0;
    while (data->token == FACET_token && parsing_can_continue(data)) {
        parse_facet(data, &facet);
        if (func_add_triangle != NULL)
            func_add_triangle(creator_cookie, i_facet, &facet);
        parsing_eat_next_token(unknown_token, data);
        data->token = parsing_find_token_from_buff(&data->string_buffer);
        ++i_facet;
    }
}

static void parse_solid(gmio_stla_parse_data_t* data)
{
    parse_beginsolid(data);
    parse_facets(data);
    parse_endsolid(data);
}

int gmio_stla_read(gmio_transfer_t* trsf, gmio_stl_mesh_creator_t* creator)
{
    char fixed_buffer[GMIO_STLA_READ_STRING_MAX_LEN];
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
    parse_data.stream_iterator.strbuff.ptr = trsf->memblock.ptr;
    parse_data.stream_iterator.strbuff.max_len = trsf->memblock.size;
    parse_data.stream_iterator.cookie = &parse_data.stream_iterator_cookie;
    parse_data.stream_iterator.func_stream_read_hook =
            gmio_stream_fwd_iterator_stla_read_hook;
    gmio_string_stream_fwd_iterator_init(&parse_data.stream_iterator);

    parse_data.string_buffer.ptr = &fixed_buffer[0];
    parse_data.string_buffer.len = 0;
    parse_data.string_buffer.max_len = GMIO_STLA_READ_STRING_MAX_LEN;

    parse_data.creator = creator;

    parse_solid(&parse_data);

    if (parse_data.error)
        return GMIO_STL_ERROR_PARSING;
    if (parse_data.stream_iterator_cookie.is_stop_requested)
        return GMIO_ERROR_TRANSFER_STOPPED;
    return GMIO_ERROR_OK;
}
