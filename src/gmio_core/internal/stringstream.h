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
 *  It allows to iterate over a stream (until end is reached) as if it was a
 *  string.
 *
 *  To be used with API below.
 */
struct gmio_stringstream
{
    /*! Stream to iterate over */
    struct gmio_stream stream;

    /*! Holds contents chunk read from stream */
    struct gmio_string strbuff;

    /*! Position after last char in strbuff */
    const char* strbuff_end;

    /*! Position indicator in strbuff */
    const char* strbuff_at;

    /*! Data to be passed to callback func_stream_read */
    void* cookie;

    /*! Pointer on a function called each time next contents chunk has to be
     * read */
    size_t (*func_stream_read)(
            void* cookie, struct gmio_stream* stream, char* ptr, size_t len);
};

/*! Returns an initialized gmio_stringstream object */
struct gmio_stringstream gmio_stringstream(
        const struct gmio_stream stream,
        const struct gmio_string strbuff);

/*! Initializes position indicator */
void gmio_stringstream_init_pos(struct gmio_stringstream* sstream);

/*! Default function for gmio_stringstream::func_stream_read */
GMIO_INLINE size_t gmio_stringstream_default_func_read(
        void* cookie, struct gmio_stream* stream, char* ptr, size_t len);

/*! Returns the char where the iterator is currently pointing at */
GMIO_INLINE const char* gmio_stringstream_current_char(
        const struct gmio_stringstream* sstream);

/*! Moves on next char in stream */
GMIO_INLINE const char* gmio_stringstream_next_char(
        struct gmio_stringstream* sstream);

/*! Moves on next char in stream */
GMIO_INLINE struct gmio_stringstream* gmio_stringstream_move_next_char(
        struct gmio_stringstream* sstream);

/*! Advances iterator until the first non-space char */
GMIO_INLINE const char* gmio_stringstream_skip_ascii_spaces(
        struct gmio_stringstream* sstream);

/*! Advances iterator until the first non-space char and copies in \p str any
 *  space found */
GMIO_INLINE void gmio_stringstream_copy_ascii_spaces(
        struct gmio_stringstream* sstream, struct gmio_string* str);

/*! Error codes returned by gmio_eat_word() */
enum gmio_eat_word_error
{
    GMIO_EAT_WORD_ERROR_OK = 0,
    GMIO_EAT_WORD_ERROR_EMPTY,
    GMIO_EAT_WORD_ERROR_CAPACITY_OVERFLOW
};

/*! Advances iterator so that next word is extracted into \p str */
enum gmio_eat_word_error gmio_stringstream_eat_word(
        struct gmio_stringstream* sstream, struct gmio_string* str);

#if 0
/*! Iterate over stream while it matches input string \p str
 *
 *  Returns true if \p str was fully matched
 */
bool gmio_stringstream_checked_next_chars(
        struct gmio_stringstream* sstream, const char* str);
#endif

/*! Parses float from stringstream \p sstream */
GMIO_INLINE float gmio_stringstream_parse_float32(
        struct gmio_stringstream* sstream);

/*! Converts C string \p str to float
 *
 *  \retval 0  On success
 *  \retval -1 On error(check \c errno to see what happened)
 *
 *  TODO: move to another header
 */
GMIO_INLINE int gmio_get_float32(const char* str, float* value_ptr);

/*! Converts C string \p str to float
 *
 *  TODO: move to another header
 */
GMIO_INLINE float gmio_to_float32(const char* str);



/*
 * -- Implementation
 */

#include "c99_stdlib_compat.h"
#include "helper_stream.h"
#include "string_ascii_utils.h"
#ifdef GMIO_STRINGSTREAM_USE_FAST_ATOF
#  include "fast_atof.h"
#  include "stringstream_fast_atof.h"
#endif

#include <errno.h>

const char* gmio_stringstream_current_char(
        const struct gmio_stringstream* sstream)
{
    return sstream->strbuff_at < sstream->strbuff_end ?
                sstream->strbuff_at :
                NULL;
}

size_t gmio_stringstream_default_func_read(
        void* cookie, struct gmio_stream* stream, char* ptr, size_t len)
{
    GMIO_UNUSED(cookie);
    return gmio_stream_read(stream, ptr, 1, len);
}

const char *gmio_stringstream_next_char(struct gmio_stringstream *sstream)
{
    ++(sstream->strbuff_at);
    if (sstream->strbuff_at < sstream->strbuff_end)
        return sstream->strbuff_at;

    /* Read next chunk of data */
    sstream->strbuff_at = sstream->strbuff.ptr;
    sstream->strbuff.len =
            sstream->func_stream_read(
                sstream->cookie,
                &sstream->stream,
                sstream->strbuff.ptr,
                sstream->strbuff.max_len);
    sstream->strbuff_end = sstream->strbuff.ptr + sstream->strbuff.len;
    return sstream->strbuff.len > 0 ? sstream->strbuff.ptr : NULL;
}

struct gmio_stringstream* gmio_stringstream_move_next_char(
        struct gmio_stringstream *sstream)
{
    gmio_stringstream_next_char(sstream);
    return sstream;
}

const char* gmio_stringstream_skip_ascii_spaces(struct gmio_stringstream* sstream)
{
    const char* curr_char = gmio_stringstream_current_char(sstream);
    while (curr_char != NULL && gmio_ascii_isspace(*curr_char))
        curr_char = gmio_stringstream_next_char(sstream);
    return curr_char;
}

void gmio_stringstream_copy_ascii_spaces(
        struct gmio_stringstream* sstream, struct gmio_string* str)
{
    const char* curr_char = gmio_stringstream_current_char(sstream);
    while (curr_char != NULL
           && gmio_ascii_isspace(*curr_char)
           && str->len < str->max_len)
    {
        str->ptr[str->len] = *curr_char;
        curr_char = gmio_stringstream_next_char(sstream);
        ++str->len;
    }
}

int gmio_get_float32(const char* str, float* value_ptr)
{
#if defined(GMIO_STRINGSTREAM_USE_FAST_ATOF)
    const char* end_ptr = NULL;
    *value_ptr = fast_strtof(str, &end_ptr);
#else
    char* end_ptr = NULL;
    *value_ptr = gmio_strtof(str, &end_ptr);
#endif
    return (end_ptr == str || errno == ERANGE) ? -1 : 0;
}

float gmio_to_float32(const char* str)
{
#if defined(GMIO_STRINGSTREAM_USE_FAST_ATOF)
    return fast_atof(str);
#else
    return gmio_strtof(str, NULL);
#endif
}

float gmio_stringstream_parse_float32(struct gmio_stringstream* sstream)
{
#if defined(GMIO_STRINGSTREAM_USE_FAST_ATOF)
    return gmio_stringstream_fast_atof(sstream);
#else
    char strbuff_ptr[64];
    struct gmio_string strbuff = { strbuff_ptr, 0, sizeof(strbuff_ptr) };
    gmio_stringstream_eat_word(sstream, &strbuff);
    return (float)atof(strbuff_ptr);
#endif
}

#endif /* GMIO_INTERNAL_STRINGSTREAM_H */
