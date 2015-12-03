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

#ifndef GMIO_INTERNAL_STRINGSTREAM_H
#define GMIO_INTERNAL_STRINGSTREAM_H

#include "../global.h"
#include "../stream.h"
#include "string.h"

/*! Stream that operates on a string
 *
 *  To be used with API below.
 *  It allows to iterate over a stream (until end is reached) as if it was a
 *  string.
 */
struct gmio_stringstream
{
    struct gmio_stream stream;
    struct gmio_string strbuff;
    const char* strbuff_end; /*!< Position after last char in strbuff */
    const char* strbuff_at;  /*!< Position indicator in buffer */

    void* cookie;
    void (*func_stream_read_hook)(
            void* cookie, const struct gmio_string* strbuff);
};

/*! Initializes iterator */
void gmio_stringstream_init(struct gmio_stringstream* it);

/*! Returns the char where the iterator is currently pointing at */
GMIO_INLINE const char* gmio_stringstream_current_char(
        const struct gmio_stringstream* it);

/*! Moves on next char in stream */
GMIO_INLINE const char* gmio_stringstream_next_char(
        struct gmio_stringstream *it);

/*! Moves on next char in stream */
GMIO_INLINE struct gmio_stringstream* gmio_stringstream_move_next_char(
        struct gmio_stringstream *it);

/*! Advances iterator until the first non-space char */
GMIO_INLINE const char* gmio_stringstream_skip_ascii_spaces(
        struct gmio_stringstream* it);

/*! Advances iterator until the first non-space char and copies in \p str any
 *  space found */
GMIO_INLINE void gmio_stringstream_copy_ascii_spaces(
        struct gmio_stringstream* it, struct gmio_string* str);

/*! Error codes returned by gmio_eat_word() */
enum gmio_eat_word_error
{
    GMIO_EAT_WORD_ERROR_OK = 0,
    GMIO_EAT_WORD_ERROR_EMPTY,
    GMIO_EAT_WORD_ERROR_CAPACITY_OVERFLOW
};

/*! Advances iterator so that next word is extracted into \p str */
enum gmio_eat_word_error gmio_stringstream_eat_word(
        struct gmio_stringstream* it, struct gmio_string* str);

#if 0
/*! Iterate over stream while it matches input string \p str
 *
 *  Returns GMIO_TRUE if \p str was fully matched
 */
gmio_bool_t gmio_stringstream_checked_next_chars(
        struct gmio_stringstream* it, const char* str);
#endif

/*! Parses float from string iterator \p it */
GMIO_INLINE gmio_float32_t gmio_stringstream_parse_float32(
        struct gmio_stringstream* it);

/*! Converts C string \p str to float
 *
 *  \retval 0  On success
 *  \retval -1 On error(check \c errno to see what happened)
 *
 *  TODO: move to another header
 */
GMIO_INLINE int gmio_get_float32(const char* str, gmio_float32_t* value_ptr);

/*! Converts C string \p str to float
 *
 *  TODO: move to another header
 */
GMIO_INLINE gmio_float32_t gmio_to_float32(const char* str);



/*
 * -- Implementation
 */

#include "helper_stream.h"
#include "string_utils.h"
#ifdef GMIO_STRINGSTREAM_USE_FAST_ATOF
#  include "fast_atof.h"
#  include "stringstream_fast_atof.h"
#endif

#include <errno.h>
#include <stdlib.h>

const char* gmio_stringstream_current_char(const struct gmio_stringstream* it)
{
    return it->strbuff_at < it->strbuff_end ?
                it->strbuff_at :
                NULL;
}

const char *gmio_stringstream_next_char(struct gmio_stringstream *it)
{
    ++(it->strbuff_at);
    if (it->strbuff_at < it->strbuff_end)
        return it->strbuff_at;

    /* Read next chunk of data */
    it->strbuff_at = it->strbuff.ptr;
    it->strbuff.len =
            gmio_stream_read(
                &it->stream, it->strbuff.ptr, 1, it->strbuff.max_len);
    it->strbuff_end = it->strbuff.ptr + it->strbuff.len;
    if (it->strbuff.len > 0) {
        if (it->func_stream_read_hook != NULL)
            it->func_stream_read_hook(it->cookie, &it->strbuff);
        return it->strbuff.ptr;
    }
    return NULL;
}

struct gmio_stringstream* gmio_stringstream_move_next_char(struct gmio_stringstream *it)
{
    gmio_stringstream_next_char(it);
    return it;
}

const char* gmio_stringstream_skip_ascii_spaces(struct gmio_stringstream* it)
{
    const char* curr_char = gmio_stringstream_current_char(it);
    while (curr_char != NULL && gmio_ascii_isspace(*curr_char))
        curr_char = gmio_stringstream_next_char(it);
    return curr_char;
}

void gmio_stringstream_copy_ascii_spaces(
        struct gmio_stringstream* it, struct gmio_string* str)
{
    const char* curr_char = gmio_stringstream_current_char(it);
    while (curr_char != NULL
           && gmio_ascii_isspace(*curr_char)
           && str->len < str->max_len)
    {
        str->ptr[str->len] = *curr_char;
        curr_char = gmio_stringstream_next_char(it);
        ++str->len;
    }
}

int gmio_get_float32(const char* str, gmio_float32_t* value_ptr)
{
#if defined(GMIO_STRINGSTREAM_USE_FAST_ATOF)
    const char* end_ptr = NULL;
    *value_ptr = fast_strtof(str, &end_ptr);
#elif defined(GMIO_HAVE_STRTOF_FUNC) /* Requires C99 */
    char* end_ptr = NULL;
    *value_ptr = strtof(str, &end_ptr);
#else
    char* end_ptr = NULL;
    *value_ptr = (gmio_float32_t)strtod(str, &end_ptr);
#endif
    return (end_ptr == str || errno == ERANGE) ? -1 : 0;
}

gmio_float32_t gmio_to_float32(const char* str)
{
#if defined(GMIO_STRINGSTREAM_USE_FAST_ATOF)
    return fast_atof(str);
#elif defined(GMIO_HAVE_STRTOF_FUNC) /* Requires C99 */
    return strtof(str, NULL);
#else
    return (gmio_float32_t)strtod(str, NULL);
#endif
}

gmio_float32_t gmio_stringstream_parse_float32(struct gmio_stringstream* it)
{
#if defined(GMIO_STRINGSTREAM_USE_FAST_ATOF)
    return gmio_stringstream_fast_atof(it);
#else
    char strbuff_ptr[64];
    struct gmio_string strbuff = { &strbuff_ptr[0], 0, sizeof(strbuff_ptr) };
    gmio_stringstream_eat_word(it, &strbuff);
    return (gmio_float32_t)atof(strbuff_ptr);
#endif
}

#endif /* GMIO_INTERNAL_STRINGSTREAM_H */
