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
#include "../gmio_core/rwargs.h"
#include "../gmio_core/internal/helper_rwargs.h"
#include "../gmio_core/internal/helper_stream.h"
#include "../gmio_core/internal/min_max.h"
#include "../gmio_core/internal/stringstream.h"
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

/* struct gmio_stringstream_stla_cookie */
struct gmio_stringstream_stla_cookie
{
    /* Copy of gmio_stla_read() corresponding argument */
    struct gmio_rwargs* rwargs;
    /* Cache for gmio_stream_size(&transfer->stream) */
    gmio_streamsize_t stream_size;
    /* Offset (in bytes) from beginning of stream : current position */
    gmio_streamoffset_t stream_offset;
    /* Cache for gmio_transfer::func_is_stop_requested() */
    gmio_bool_t is_stop_requested;
};

/* gmio_stla_parse_data */
struct gmio_stla_parse_data
{
    enum gmio_stla_token token;
    gmio_bool_t error;
    struct gmio_stringstream strstream;
    struct gmio_stringstream_stla_cookie strstream_cookie;
    struct gmio_string string_buffer;
    struct gmio_stl_mesh_creator* creator;
};

/* Fixed maximum length of any gmio_string in this source file */
enum { GMIO_STLA_READ_STRING_MAX_LEN = 1024 };

/* Callback used for gmio_string_stream_fwd_iterator::func_stream_read_hook */
static void gmio_stringstream_stla_read_hook(
        void* cookie, const struct gmio_string* strbuff)
{
    struct gmio_stringstream_stla_cookie* tcookie =
            (struct gmio_stringstream_stla_cookie*)(cookie);
    const struct gmio_rwargs* rwargs =
            tcookie != NULL ? tcookie->rwargs : NULL;
    if (tcookie != NULL) {
        tcookie->stream_offset += strbuff->len;
        tcookie->is_stop_requested = gmio_rwargs_is_stop_requested(rwargs);
        gmio_rwargs_handle_progress(
                    rwargs, tcookie->stream_offset, tcookie->stream_size);
    }
}

/* Root function, parses a whole solid */
static void parse_solid(struct gmio_stla_parse_data* data);

int gmio_stla_read(
        struct gmio_rwargs* args,
        struct gmio_stl_mesh_creator* creator)
{
    char fixed_buffer[GMIO_STLA_READ_STRING_MAX_LEN];
    struct gmio_stla_parse_data parse_data;

    { /* Check validity of input parameters */
        int error = GMIO_ERROR_OK;
        if (!gmio_check_rwargs(&error, args))
            return error;
    }

    parse_data.token = unknown_token;
    parse_data.error = GMIO_FALSE;

    parse_data.strstream_cookie.rwargs = args;
    parse_data.strstream_cookie.stream_offset = 0;
    parse_data.strstream_cookie.stream_size =
            gmio_stream_size(&args->stream);
    parse_data.strstream_cookie.is_stop_requested = GMIO_FALSE;

    parse_data.strstream.stream = args->stream;
    parse_data.strstream.strbuff.ptr = args->memblock.ptr;
    parse_data.strstream.strbuff.max_len = args->memblock.size;
    parse_data.strstream.cookie = &parse_data.strstream_cookie;
    parse_data.strstream.func_stream_read_hook =
            gmio_stringstream_stla_read_hook;
    gmio_stringstream_init(&parse_data.strstream);

    parse_data.string_buffer.ptr = &fixed_buffer[0];
    parse_data.string_buffer.len = 0;
    parse_data.string_buffer.max_len = GMIO_STLA_READ_STRING_MAX_LEN;

    parse_data.creator = creator;

    parse_solid(&parse_data);

    if (parse_data.error)
        return GMIO_STL_ERROR_PARSING;
    if (parse_data.strstream_cookie.is_stop_requested)
        return GMIO_ERROR_TRANSFER_STOPPED;
    return GMIO_ERROR_OK;
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

/* Qualifies input string as a token */
static enum gmio_stla_token stla_find_token(const char* word, size_t word_len);

/* Same as parsing_find_token() but takes a struct gmio_string object as input */
GMIO_INLINE enum gmio_stla_token stla_find_token_from_string(const struct gmio_string* str);

/* Returns true if \p token matches one of the candidate tokens
 *
 * Array \p candidates must end with a "null_token" item
 */
GMIO_INLINE gmio_bool_t stla_token_match_candidate(
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

/* Eats next token string and checks it against an expected token
 *
 * This procedure copies the token string into internal
 * struct gmio_stla_parse_data::string_buffer
 */
static int stla_eat_next_token(
        struct gmio_stla_parse_data* data, enum gmio_stla_token expected_token);

/* Eats next token string and checks it against an expected token
 *
 * This procedure does the same thing as parsing_eat_next_token() but is faster
 * as it does not copy the token string(it just "reads" it).
 * It performs "in-place" case insensitive string comparison of the current
 * token string against expected
 */
static int stla_eat_next_token_inplace(
        struct gmio_stla_parse_data* data, enum gmio_stla_token expected_token);

/* Eats contents until some expected "end" token is matched
 *
 * Array \p end_tokens must end with a "null_token" item
 */
static int stla_eat_until_token(
        struct gmio_stla_parse_data* data, const enum gmio_stla_token* end_tokens);

/* Returns true if parsing can continue */
GMIO_INLINE gmio_bool_t stla_parsing_can_continue(
        const struct gmio_stla_parse_data* data);

/* --------------------------------------------------------------------------
 * STLA parsing functions
 * -------------------------------------------------------------------------- */

/* Used as general STLA parsing error code */
enum { GMIO_STLA_PARSE_ERROR = 1 };

/* Parses the (optional) solid name that appears after token "solid" */
static int parse_solidname_beg(struct gmio_stla_parse_data* data);

/* Parses the (optional) solid name that appears after token "endsolid"
 *
 * It should be the same name as the one parsed with parse_solidname_beg()
 */
static int parse_solidname_end(struct gmio_stla_parse_data* data);

/* Parses "solid <name>" */
static int parse_beginsolid(struct gmio_stla_parse_data* data);

/* Parses "endsolid <name>" */
static gmio_bool_t parse_endsolid(struct gmio_stla_parse_data* data);

/* Parses STL (x,y,z) coords, each coord being separated by whitespaces */
GMIO_INLINE int parse_xyz_coords(
        struct gmio_stla_parse_data* data, struct gmio_stl_coords* coords);

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

enum gmio_stla_token stla_find_token(const char* word, size_t word_len)
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
    return stla_find_token(str->ptr, str->len);
}

gmio_bool_t stla_token_match_candidate(
        enum gmio_stla_token token, const enum gmio_stla_token* candidates)
{
    gmio_bool_t found = GMIO_FALSE;
    size_t i;
    for (i = 0; !found && candidates[i] != null_token; ++i)
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
            "                 current token: <%s>\n"
            "                 current token string: \"%s\"\n",
            msg,
            stla_token_to_string(data->token),
            data->string_buffer.ptr);
    data->error = GMIO_TRUE;
    data->token = unknown_token;
}

void stla_error_token_expected(
        struct gmio_stla_parse_data* data, enum gmio_stla_token token)
{
    char msg[256] = {0};
    sprintf(msg,
            "token <%s> expected, got <%s>",
            stla_token_to_string(token),
            stla_token_to_string(data->token));
    stla_error_msg(data, msg);
}

/* --------------------------------------------------------------------------
 * Parsing helper functions
 * -------------------------------------------------------------------------- */

int stla_eat_next_token(
        struct gmio_stla_parse_data* data,
        enum gmio_stla_token expected_token)
{
    struct gmio_string* strbuff = &data->string_buffer;
    enum gmio_eat_word_error eat_error;

    strbuff->len = 0;
    eat_error = gmio_stringstream_eat_word(&data->strstream, strbuff);
    if (eat_error == GMIO_EAT_WORD_ERROR_OK) {
        const char* expected_token_str = stla_token_to_string(expected_token);
        if (gmio_ascii_stricmp(strbuff->ptr, expected_token_str) == 0) {
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

int stla_eat_next_token_inplace(
        struct gmio_stla_parse_data* data,
        enum gmio_stla_token expected_token)
{
    struct gmio_stringstream* sstream = &data->strstream;
    const char* stream_char = NULL;
    const char* expected_token_str = stla_token_to_string(expected_token);
    gmio_bool_t error = GMIO_FALSE;

    data->token = unknown_token;
    stream_char = gmio_stringstream_skip_ascii_spaces(sstream);
    while (!error) {
        if (stream_char == NULL || gmio_ascii_isspace(*stream_char)) {
            if (*expected_token_str == 0) {
                data->token = expected_token;
                return 0;
            }
            error = GMIO_TRUE;
        }
        else if (!gmio_ascii_char_iequals(*stream_char, *expected_token_str)
                 || *expected_token_str == 0)
        {
            error = GMIO_TRUE;
        }
        stream_char = gmio_stringstream_next_char(sstream);
        ++expected_token_str;
    }

    if (error) {
        stla_error_token_expected(data, expected_token);
        return GMIO_STLA_PARSE_ERROR;
    }
    return 0;
}

int stla_eat_until_token(
        struct gmio_stla_parse_data* data, const enum gmio_stla_token* end_tokens)
{
    if (!stla_token_match_candidate(data->token, end_tokens)) {
        struct gmio_stringstream* sstream = &data->strstream;
        struct gmio_string* strbuff = &data->string_buffer;
        gmio_bool_t end_token_found = GMIO_FALSE;

        do {
            const size_t previous_buff_len = strbuff->len;
            enum gmio_eat_word_error eat_word_err = 0;
            const char* next_word = NULL; /* Pointer on next word string */
            size_t next_word_len = 0; /* Length of next word string */

            gmio_stringstream_copy_ascii_spaces(sstream, strbuff);
            /* Next word */
            next_word = strbuff->ptr + strbuff->len;
            eat_word_err = gmio_stringstream_eat_word(sstream, strbuff);
            next_word_len = (strbuff->ptr + strbuff->len) - next_word;
            /* Qualify token */
            data->token =
                    eat_word_err == GMIO_EAT_WORD_ERROR_OK ?
                        stla_find_token(next_word, next_word_len) :
                        unknown_token;
            /* End token found ? */
            end_token_found = stla_token_match_candidate(data->token, end_tokens);
            /* True ?
             * trim string_buf so it contains only contents before end token */
            if (end_token_found) {
                strbuff->len = previous_buff_len;
                strbuff->ptr[previous_buff_len] = 0;
            }
        } while (!end_token_found && strbuff->len < strbuff->max_len);

        if (!end_token_found) {
            stla_error_msg(
                        data, "end token not found in parse_eat_until_token()");
            return GMIO_STLA_PARSE_ERROR;
        }
    }
    return 0;
}

gmio_bool_t stla_parsing_can_continue(const struct gmio_stla_parse_data* data)
{
    return !data->error && !data->strstream_cookie.is_stop_requested;
}

/* --------------------------------------------------------------------------
 * STLA parsing functions
 * -------------------------------------------------------------------------- */

int parse_solidname_beg(struct gmio_stla_parse_data* data)
{
    if (stla_eat_next_token(data, unknown_token) == 0) {
        data->token = stla_find_token_from_string(&data->string_buffer);
        if (data->token == FACET_token || data->token == ENDSOLID_token) {
            gmio_string_clear(&data->string_buffer);
            return 0;
        }
        else {
            /* Solid name can be made of multiple words */
            const enum gmio_stla_token end_tokens[] = {
                FACET_token, ENDSOLID_token, null_token };
            return stla_eat_until_token(data, end_tokens);
        }
    }
    return GMIO_STLA_PARSE_ERROR;
}

int parse_solidname_end(struct gmio_stla_parse_data* data)
{
    GMIO_UNUSED(data);
    /* TODO: parse according to retrieved solid name */
    return 0;
}

int parse_beginsolid(struct gmio_stla_parse_data* data)
{
    if (stla_eat_next_token(data, SOLID_token) == 0) {
        if (parse_solidname_beg(data) == 0) {
            gmio_stl_mesh_creator_ascii_begin_solid(
                        data->creator,
                        data->strstream_cookie.stream_size,
                        data->string_buffer.ptr);
            return 0;
        }
    }
    return GMIO_STLA_PARSE_ERROR;
}

gmio_bool_t parse_endsolid(struct gmio_stla_parse_data* data)
{
    if (data->token == ENDSOLID_token
            || stla_eat_next_token(data, ENDSOLID_token) == 0)
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

int parse_xyz_coords(struct gmio_stla_parse_data* data, struct gmio_stl_coords* coords)
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

    data->string_buffer.len = 0;
    data->token = unknown_token;

    return errc;
}

int parse_facet(
        struct gmio_stla_parse_data* data,
        struct gmio_stl_triangle* facet)
{
    int errc = 0;
    if (data->token != FACET_token)
        errc += stla_eat_next_token_inplace(data, FACET_token);

    errc += stla_eat_next_token_inplace(data, NORMAL_token);
    parse_xyz_coords(data, &facet->normal);

    errc += stla_eat_next_token_inplace(data, OUTER_token);
    errc += stla_eat_next_token_inplace(data, LOOP_token);

    errc += stla_eat_next_token_inplace(data, VERTEX_token);
    errc += parse_xyz_coords(data, &facet->v1);
    errc += stla_eat_next_token_inplace(data, VERTEX_token);
    errc += parse_xyz_coords(data, &facet->v2);
    errc += stla_eat_next_token_inplace(data, VERTEX_token);
    errc += parse_xyz_coords(data, &facet->v3);

    errc += stla_eat_next_token_inplace(data, ENDLOOP_token);
    errc += stla_eat_next_token_inplace(data, ENDFACET_token);

    return errc;
}

void parse_facets(struct gmio_stla_parse_data* data)
{
    const gmio_stl_mesh_creator_func_add_triangle_t func_add_triangle =
            data->creator->func_add_triangle;
    void* creator_cookie = data->creator->cookie;
    struct gmio_string* strbuff = &data->string_buffer;
    uint32_t i_facet = 0;
    struct gmio_stl_triangle facet;

    facet.attribute_byte_count = 0;
    while (data->token == FACET_token && stla_parsing_can_continue(data)) {
        if (parse_facet(data, &facet) == 0) {
            /* Add triangle to user mesh */
            if (func_add_triangle != NULL)
                func_add_triangle(creator_cookie, i_facet, &facet);
            /* Eat next unknown token */
            strbuff->len = 0;
            gmio_stringstream_eat_word(&data->strstream, strbuff);
            data->token = stla_find_token_from_string(strbuff);
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
