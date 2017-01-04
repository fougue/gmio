/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#include "stl_io.h"

#include "stl_error.h"
#include "internal/helper_stl_mesh_creator.h"
#include "internal/stl_funptr_typedefs.h"
#include "internal/stl_error_check.h"
#include "internal/stla_parsing.h"

#include "../gmio_core/const_string.h"
#include "../gmio_core/error.h"
#include "../gmio_core/internal/c99_stdio_compat.h"
#include "../gmio_core/internal/error_check.h"
#include "../gmio_core/internal/helper_memblock.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/helper_task_iface.h"
#include "../gmio_core/internal/min_max.h"
#include "../gmio_core/internal/safe_cast.h"
#include "../gmio_core/internal/stringstream.h"
#include "../gmio_core/internal/string_ascii_utils.h"

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


/* Callback used for gmio_stringstream::func_stream_read */
static size_t gmio_stringstream_stla_read(
        void* cookie, struct gmio_stream* stream, char* ptr, size_t len)
{
    struct gmio_stringstream_stla_cookie* stlac =
            (struct gmio_stringstream_stla_cookie*)(cookie);
    if (stlac != NULL) {
        const struct gmio_task_iface* task = stlac->task;
        const size_t remaining_contents_size =
                gmio_streamsize_to_size(
                    stlac->stream_size - stlac->stream_offset + 1);
        const size_t to_read = GMIO_MIN(len, remaining_contents_size);
        const size_t len_read = gmio_stream_read_bytes(stream, ptr, to_read);
        stlac->stream_offset += len_read;
        stlac->is_stop_requested = gmio_task_iface_is_stop_requested(task);
        gmio_task_iface_handle_progress(
                task, stlac->stream_offset, stlac->stream_size);
        return len_read;
    }
    return 0;
}

/* Root function, parses a whole solid */
static void parse_solid(struct gmio_stla_parse_data* data);

int gmio_stla_read(
        struct gmio_stream* stream,
        struct gmio_stl_mesh_creator* mesh_creator,
        const struct gmio_stl_read_options* opts)
{
    /* Constants */
    static const struct gmio_stl_read_options default_opts = {0};
    /* Variables */
    struct gmio_memblock_helper mblock_helper =
            gmio_memblock_helper(opts != NULL ? &opts->stream_memblock : NULL);
    struct gmio_memblock* const mblock = &mblock_helper.memblock;
    char fixed_buffer[GMIO_STLA_READ_STRING_MAX_LEN];
    struct gmio_stla_parse_data parse_data;
    int error = GMIO_ERROR_OK;

    /* Make options non NULL */
    opts = opts != NULL ? opts : &default_opts;

    /* Check validity of input parameters */
    if (!opts->stla_dont_check_lc_numeric && !gmio_check_lc_numeric(&error))
        goto label_end;

    /* Initialize helper gmio_stla_parse_data object */
    parse_data.token = unknown_token;
    parse_data.error = false;

    parse_data.strstream_cookie.task = &opts->task_iface;
    parse_data.strstream_cookie.stream_offset = 0;

    parse_data.strstream_cookie.stream_size =
            opts->func_stla_get_streamsize != NULL ?
                opts->func_stla_get_streamsize(stream, mblock) :
                gmio_stream_size(stream);
    parse_data.strstream_cookie.is_stop_requested = false;

    parse_data.strstream.stream = *stream;
    parse_data.strstream.strbuff.ptr = mblock->ptr;
    parse_data.strstream.strbuff.capacity = mblock->size;
    parse_data.strstream.cookie = &parse_data.strstream_cookie;
    parse_data.strstream.func_stream_read = gmio_stringstream_stla_read;
    gmio_stringstream_init_pos(&parse_data.strstream);

    parse_data.token_str = gmio_string(fixed_buffer, 0, sizeof(fixed_buffer));

    parse_data.creator = mesh_creator;

    parse_solid(&parse_data);

    if (parse_data.error)
        error = GMIO_STL_ERROR_PARSING;
    if (parse_data.strstream_cookie.is_stop_requested)
        error = GMIO_ERROR_TASK_STOPPED;

label_end:
    gmio_memblock_helper_release(&mblock_helper);
    return error;
}



/* __________________________________________________________________________
 *
 * Private API
 * __________________________________________________________________________ */

/* --------------------------------------------------------------------------
 * STLA token utils
 * -------------------------------------------------------------------------- */

static const char stla_tokstr_ENDFACET[] = "endfacet";
static const char stla_tokstr_ENDLOOP[] = "endloop";
static const char stla_tokstr_ENDSOLID[] = "endsolid";
static const char stla_tokstr_FACET[] = "facet";
static const char stla_tokstr_LOOP[] = "loop";
static const char stla_tokstr_NORMAL[] = "normal";
static const char stla_tokstr_OUTER[] = "outer";
static const char stla_tokstr_SOLID[] = "solid";
static const char stla_tokstr_VERTEX[] = "vertex";

static const struct gmio_const_string stla_tokcstr[] = {
    {0}, /* null_token */
    GMIO_CONST_STRING_FROM_ARRAY(stla_tokstr_ENDFACET),
    GMIO_CONST_STRING_FROM_ARRAY(stla_tokstr_ENDLOOP),
    GMIO_CONST_STRING_FROM_ARRAY(stla_tokstr_ENDSOLID),
    GMIO_CONST_STRING_FROM_ARRAY(stla_tokstr_FACET),
    GMIO_CONST_STRING_FROM_ARRAY(stla_tokstr_LOOP),
    GMIO_CONST_STRING_FROM_ARRAY(stla_tokstr_NORMAL),
    GMIO_CONST_STRING_FROM_ARRAY(stla_tokstr_OUTER),
    GMIO_CONST_STRING_FROM_ARRAY(stla_tokstr_SOLID),
    GMIO_CONST_STRING_FROM_ARRAY(stla_tokstr_VERTEX),
    { "ID", 2 }, /* ID_token */
    { "",   0 }, /* empty_token */
    { "?",  1 }  /* unknown_token */
};

/* Returns the string corresponding to token */
GMIO_INLINE const char* stla_token_to_string(enum gmio_stla_token token);

/* Same as parsing_find_token() but takes a struct gmio_string object as input */
GMIO_INLINE enum gmio_stla_token stla_find_token_from_string(const struct gmio_string* str);

/* Returns true if \p token matches one of the candidate tokens
 *
 * Array \p candidates must end with a "null_token" item
 */
GMIO_INLINE bool stla_token_match_candidate(
        enum gmio_stla_token token, const enum gmio_stla_token* candidates);

/* --------------------------------------------------------------------------
 * Error message functions
 * -------------------------------------------------------------------------- */

/* Makes the parsing fails and print error message */
static void stla_error_msg(
        struct gmio_stla_parse_data* data, const char* msg);

/* Makes the parsing fails with message showing token mismatch */
static void stla_error_token_expected(
        struct gmio_stla_parse_data* data, enum gmio_stla_token token);

/* --------------------------------------------------------------------------
 * Parsing helper functions
 * -------------------------------------------------------------------------- */

/* Returns true if parsing can continue */
GMIO_INLINE bool stla_parsing_can_continue(
        const struct gmio_stla_parse_data* data);

/* --------------------------------------------------------------------------
 * STLA parsing functions
 * -------------------------------------------------------------------------- */

/* Parses the (optional) solid name that appears after token "endsolid"
 *
 * It should be the same name as the one parsed with
 * gmio_stla_parse_solidname_beg()
 */
static int parse_solidname_end(struct gmio_stla_parse_data* data);

/* Parses "solid <name>" */
static int parse_beginsolid(struct gmio_stla_parse_data* data);

/* Parses "endsolid <name>" */
static bool parse_endsolid(struct gmio_stla_parse_data* data);

/* Parses STL (x,y,z) coords, each coord being separated by whitespaces */
GMIO_INLINE int parse_xyz_coords(
        struct gmio_stla_parse_data* data, struct gmio_vec3f* coords);

/* Parses a STL facet, ie. facet ... endfacet */
static int parse_facet(
        struct gmio_stla_parse_data* data, struct gmio_stl_triangle* facet);

/* Parses a list of facets */
static void parse_facets(struct gmio_stla_parse_data* data);

/* __________________________________________________________________________
 *
 * Private API implementation
 * __________________________________________________________________________ */

/* --------------------------------------------------------------------------
 * STLA token utils
 * -------------------------------------------------------------------------- */

const char* stla_token_to_string(enum gmio_stla_token token)
{
    return stla_tokcstr[token].ptr;
}

enum gmio_stla_token gmio_stla_find_token(const char* word, size_t word_len)
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

enum gmio_stla_token stla_find_token_from_string(const struct gmio_string* str)
{
    return gmio_stla_find_token(str->ptr, str->len);
}

bool stla_token_match_candidate(
        enum gmio_stla_token token, const enum gmio_stla_token* candidates)
{
    bool found = false;
    for (size_t i = 0; !found && candidates[i] != null_token; ++i)
        found = token == candidates[i];
    return found;
}

/* --------------------------------------------------------------------------
 * Error message functions
 * -------------------------------------------------------------------------- */

void stla_error_msg(struct gmio_stla_parse_data* data, const char* msg)
{
    fprintf(stderr,
            "\n"
            "gmio_stla_read() parsing error: %s\n"
            "                 current token string: \"%s\"\n",
            msg,
            data->token_str.ptr);
    data->error = true;
    data->token = unknown_token;
}

void stla_error_token_expected(
        struct gmio_stla_parse_data* data, enum gmio_stla_token token)
{
    char msg[256] = {0};
    gmio_snprintf(msg, sizeof(msg),
                  "expected <%s>, got <%s>",
                  stla_token_to_string(token),
                  stla_token_to_string(data->token));
    stla_error_msg(data, msg);
}

/* --------------------------------------------------------------------------
 * Parsing helper functions
 * -------------------------------------------------------------------------- */

int gmio_stla_eat_next_token(
        struct gmio_stla_parse_data* data,
        enum gmio_stla_token expected_token)
{
    struct gmio_string* token_str = &data->token_str;
    token_str->len = 0;
    const enum gmio_eat_word_error eat_error =
            gmio_stringstream_eat_word(&data->strstream, token_str);
    if (eat_error == GMIO_EAT_WORD_ERROR_OK) {
        const char* expected_token_str = stla_token_to_string(expected_token);
        if (gmio_ascii_stricmp(token_str->ptr, expected_token_str) == 0) {
            data->token = expected_token;
            return 0;
        }
        data->token = unknown_token;
        if (expected_token == unknown_token)
            return 0;
        stla_error_token_expected(data, expected_token);
        return GMIO_STLA_PARSE_ERROR;
    }
    else {
        stla_error_msg(data, "failure to get next word with gmio_eat_word()");
        return GMIO_STLA_PARSE_ERROR;
    }
}

int gmio_stla_eat_next_token_inplace(
        struct gmio_stla_parse_data* data,
        enum gmio_stla_token expected_token)
{
    struct gmio_stringstream* sstream = &data->strstream;
    const char* expected_token_str = stla_token_to_string(expected_token);
    bool error = false;

    data->token = unknown_token;
    const char* stream_char = gmio_stringstream_skip_ascii_spaces(sstream);
    while (!error) {
        if (stream_char == NULL || gmio_ascii_isspace(*stream_char)) {
            if (*expected_token_str == 0) {
                data->token = expected_token;
                return 0; /* Success */
            }
            error = true;
        }
        else if (!gmio_ascii_char_iequals(*stream_char, *expected_token_str)
                 || *expected_token_str == 0)
        {
            error = true;
        }
        else {
            stream_char = gmio_stringstream_next_char(sstream);
            ++expected_token_str;
        }
    }

    /* Error, copy the wrong token in data->token_str */
    {
        size_t i = 0;
        /* -- Copy the matching part of the expected token */
        const char* it = stla_token_to_string(expected_token);
        while (it != expected_token_str)
            data->token_str.ptr[i++] = *(it++);
        /* -- Copy the non matching part */
        while (i < data->token_str.capacity
               && stream_char != NULL
               && !gmio_ascii_isspace(*stream_char))
        {
            data->token_str.ptr[i++] = *stream_char;
            stream_char = gmio_stringstream_next_char(sstream);
        }
        /* -- Fill remaining space with NUL byte */
        memset(data->token_str.ptr + i, '\0', data->token_str.capacity - i);
        data->token_str.len = i;
        data->token = stla_find_token_from_string(&data->token_str);

    }

    /* Notify error */
    stla_error_token_expected(data, expected_token);
    return GMIO_STLA_PARSE_ERROR;
}

int gmio_stla_eat_until_token(
        struct gmio_stla_parse_data* data,
        const enum gmio_stla_token* end_tokens)
{
    if (!stla_token_match_candidate(data->token, end_tokens)) {
        struct gmio_stringstream* sstream = &data->strstream;
        struct gmio_string* strbuff = &data->token_str;
        bool end_token_found = false;

        do {
            const size_t previous_buff_len = strbuff->len;
            gmio_stringstream_copy_ascii_spaces(sstream, strbuff);
            /* Next word */
            const char* next_word = strbuff->ptr + strbuff->len;
            const enum gmio_eat_word_error eat_word_err =
                    gmio_stringstream_eat_word(sstream, strbuff);
            const size_t next_word_len =
                    (strbuff->ptr + strbuff->len) - next_word;
            /* Qualify token */
            data->token =
                    eat_word_err == GMIO_EAT_WORD_ERROR_OK ?
                        gmio_stla_find_token(next_word, next_word_len) :
                        unknown_token;
            /* End token found ? */
            end_token_found = stla_token_match_candidate(data->token, end_tokens);
            /* True ?
             * trim string_buf so it contains only contents before end token */
            if (end_token_found) {
                strbuff->len = previous_buff_len;
                strbuff->ptr[previous_buff_len] = 0;
            }
        } while (!end_token_found && strbuff->len < strbuff->capacity);

        if (!end_token_found) {
            stla_error_msg(
                        data, "end token not found in parse_eat_until_token()");
            return GMIO_STLA_PARSE_ERROR;
        }
    }
    return 0;
}

bool stla_parsing_can_continue(const struct gmio_stla_parse_data* data)
{
    return !data->error && !data->strstream_cookie.is_stop_requested;
}

/* --------------------------------------------------------------------------
 * STLA parsing functions
 * -------------------------------------------------------------------------- */

int gmio_stla_parse_solidname_beg(struct gmio_stla_parse_data* data)
{
    if (gmio_stla_eat_next_token(data, unknown_token) == 0) {
        data->token = stla_find_token_from_string(&data->token_str);
        if (data->token == FACET_token || data->token == ENDSOLID_token) {
            gmio_string_clear(&data->token_str);
            return 0;
        }
        else {
            /* Solid name can be made of multiple words */
            const enum gmio_stla_token end_tokens[] = {
                FACET_token, ENDSOLID_token, null_token };
            return gmio_stla_eat_until_token(data, end_tokens);
        }
    }
    return GMIO_STLA_PARSE_ERROR;
}

int parse_solidname_end(struct gmio_stla_parse_data* data)
{
    struct gmio_stringstream* sstream = &data->strstream;
    /* Eat whole line after "endsolid" */
    const char* c = gmio_stringstream_current_char(sstream);
    while (c != NULL && *c != '\n' && *c != '\r')
        c = gmio_stringstream_next_char(sstream);
    return 0;
}

int parse_beginsolid(struct gmio_stla_parse_data* data)
{
    if (gmio_stla_eat_next_token(data, SOLID_token) == 0) {
        if (gmio_stla_parse_solidname_beg(data) == 0) {
            struct gmio_stl_mesh_creator_infos infos = {0};
            infos.format = GMIO_STL_FORMAT_ASCII;
            infos.stla_solid_name = data->token_str.ptr;
            infos.stla_stream_size = data->strstream_cookie.stream_size;
            gmio_stl_mesh_creator_begin_solid(data->creator, &infos);
            return 0;
        }
    }
    return GMIO_STLA_PARSE_ERROR;
}

bool parse_endsolid(struct gmio_stla_parse_data* data)
{
    if (data->token == ENDSOLID_token
            || gmio_stla_eat_next_token(data, ENDSOLID_token) == 0)
    {
        parse_solidname_end(data);
        gmio_stl_mesh_creator_end_solid(data->creator);
        return 0;
    }
    return GMIO_STLA_PARSE_ERROR;
}

/* Returns true if first char of input string is valid for float numbers */
GMIO_INLINE int is_float_char(const char* str)
{
    const char c = str != NULL ? *str : 0;
    return gmio_ascii_isdigit(c)
            || c == '-'
            || c == '.'
            || c == 'e'
            || c == 'E'
            || c == '+';
}

int parse_xyz_coords(struct gmio_stla_parse_data* data, struct gmio_vec3f* coords)
{
    int errc = 0;
    struct gmio_stringstream* sstream = &data->strstream;
    const char* strbuff = NULL;

    strbuff = gmio_stringstream_skip_ascii_spaces(sstream);
    errc += !is_float_char(strbuff);
    coords->x = gmio_stringstream_parse_float32(sstream);

    strbuff = gmio_stringstream_skip_ascii_spaces(sstream);
    errc += !is_float_char(strbuff);
    coords->y = gmio_stringstream_parse_float32(sstream);

    strbuff = gmio_stringstream_skip_ascii_spaces(sstream);
    errc += !is_float_char(strbuff);
    coords->z = gmio_stringstream_parse_float32(sstream);

    data->token_str.len = 0;
    data->token = unknown_token;

    return errc;
}

int parse_facet(
        struct gmio_stla_parse_data* data,
        struct gmio_stl_triangle* facet)
{
    int errc = 0;
    if (data->token != FACET_token)
        errc += gmio_stla_eat_next_token_inplace(data, FACET_token);

    errc += gmio_stla_eat_next_token_inplace(data, NORMAL_token);
    parse_xyz_coords(data, &facet->n);

    errc += gmio_stla_eat_next_token_inplace(data, OUTER_token);
    errc += gmio_stla_eat_next_token_inplace(data, LOOP_token);

    errc += gmio_stla_eat_next_token_inplace(data, VERTEX_token);
    errc += parse_xyz_coords(data, &facet->v1);
    errc += gmio_stla_eat_next_token_inplace(data, VERTEX_token);
    errc += parse_xyz_coords(data, &facet->v2);
    errc += gmio_stla_eat_next_token_inplace(data, VERTEX_token);
    errc += parse_xyz_coords(data, &facet->v3);

    errc += gmio_stla_eat_next_token_inplace(data, ENDLOOP_token);
    errc += gmio_stla_eat_next_token_inplace(data, ENDFACET_token);

    return errc;
}

void parse_facets(struct gmio_stla_parse_data* data)
{
    const gmio_stl_mesh_creator_func_add_triangle_t func_add_triangle =
            data->creator->func_add_triangle;
    void* creator_cookie = data->creator->cookie;
    struct gmio_string* token_str = &data->token_str;
    uint32_t i_facet = 0;
    struct gmio_stl_triangle facet;

    facet.attribute_byte_count = 0;
    while (data->token == FACET_token && stla_parsing_can_continue(data)) {
        if (parse_facet(data, &facet) == 0) {
            /* Add triangle to user mesh */
            if (func_add_triangle != NULL)
                func_add_triangle(creator_cookie, i_facet, &facet);
            /* Eat next unknown token */
            token_str->len = 0;
            gmio_stringstream_eat_word(&data->strstream, token_str);
            data->token = stla_find_token_from_string(token_str);
            ++i_facet;
        }
        else {
            stla_error_msg(data, "Invalid facet");
        }
    }
}

void parse_solid(struct gmio_stla_parse_data* data)
{
    parse_beginsolid(data);
    parse_facets(data);
    parse_endsolid(data);
}
