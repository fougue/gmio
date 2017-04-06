/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
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

#pragma once

#include "../../gmio_core/global.h"
#include "../../gmio_core/stream.h"
#include "../../gmio_core/internal/stringstream.h"

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
    const struct gmio_task_iface* task;
    /* Cache for the input stream size */
    gmio_streamsize_t stream_size;
    /* Offset (in bytes) from beginning of stream : current position */
    gmio_streamoffset_t stream_offset;
    /* Cache for gmio_task_iface::func_is_stop_requested() */
    bool is_stop_requested;
};

/* gmio_stla_parse_data */
struct gmio_stla_parse_data
{
    /* Current token type */
    enum gmio_stla_token token;
    /* Current token string */
    struct gmio_string token_str;
    /* Any actual error ? */
    bool error;
    /* The stringstream object used to read the input stream */
    struct gmio_stringstream strstream;
    /* The cookie in strstream */
    struct gmio_stringstream_stla_cookie strstream_cookie;
    /* The mesh creator callbacks */
    struct gmio_stl_mesh_creator* creator;
};

/* Fixed maximum length of any gmio_string when parsing */
enum { GMIO_STLA_READ_STRING_MAX_LEN = 1024 };

/* Used as general STLA parsing error code */
enum { GMIO_STLA_PARSE_ERROR = 1 };

/* --------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------- */

/* Qualifies input string as a token */
enum gmio_stla_token gmio_stla_find_token(const char* word, size_t word_len);

/* Parses the (optional) solid name that appears after token "solid" */
int gmio_stla_parse_solidname_beg(struct gmio_stla_parse_data* data);

/* Eats next token string and checks it against an expected token
 *
 * This procedure copies the token string into internal
 * struct gmio_stla_parse_data::strbuff */
int gmio_stla_eat_next_token(
        struct gmio_stla_parse_data* data, enum gmio_stla_token expected_token);

/* Eats next token string and checks it against an expected token
 *
 * This procedure does the same thing as parsing_eat_next_token() but is faster
 * as it does not copy the token string(it just "reads" it).
 * It performs "in-place" case insensitive string comparison of the current
 * token string against expected */
int gmio_stla_eat_next_token_inplace(
        struct gmio_stla_parse_data* data, enum gmio_stla_token expected_token);

/* Eats contents until some expected "end" token is matched
 *
 * Array \p end_tokens must end with a "null_token" item */
int gmio_stla_eat_until_token(
        struct gmio_stla_parse_data* data, const enum gmio_stla_token* end_tokens);
