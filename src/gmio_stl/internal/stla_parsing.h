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

#ifndef GMIO_INTERNAL_STLA_PARSING_H
#define GMIO_INTERNAL_STLA_PARSING_H

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
 * struct gmio_stla_parse_data::strbuff
 */
int gmio_stla_eat_next_token(
        struct gmio_stla_parse_data* data, enum gmio_stla_token expected_token);

/* Eats next token string and checks it against an expected token
 *
 * This procedure does the same thing as parsing_eat_next_token() but is faster
 * as it does not copy the token string(it just "reads" it).
 * It performs "in-place" case insensitive string comparison of the current
 * token string against expected
 */
int gmio_stla_eat_next_token_inplace(
        struct gmio_stla_parse_data* data, enum gmio_stla_token expected_token);

/* Eats contents until some expected "end" token is matched
 *
 * Array \p end_tokens must end with a "null_token" item
 */
int gmio_stla_eat_until_token(
        struct gmio_stla_parse_data* data, const enum gmio_stla_token* end_tokens);

#endif /* GMIO_INTERNAL_STLA_PARSING_H */
