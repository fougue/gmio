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

    /*! Pointer on a function called each time next contents chunk has to be read */
    size_t (*func_stream_read)(
            void* cookie, struct gmio_stream* stream, char* ptr, size_t len);
};

/*! Returns an initialized stringstream */
struct gmio_stringstream gmio_stringstream(
        const struct gmio_stream stream,
        const struct gmio_string strbuff);

/*! Initializes position indicator */
void gmio_stringstream_init_pos(struct gmio_stringstream* sstream);

/*! Default function for gmio_stringstream::func_stream_read */
size_t gmio_stringstream_default_func_read(
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
/*! Iterate over stream while it matches input string \p str.
 *  Returns true if \p str was fully matched */
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
#include "string_ascii_utils.h"
#if GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_IRRLICHT
#  include "fast_atof.h"
#  include "stringstream_fast_atof.h"
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_DOUBLE_CONVERSION
#  include "google_doubleconversion.h"
#endif

#include <errno.h>

const char* gmio_stringstream_current_char(
        const struct gmio_stringstream* sstream)
{
    return sstream->strbuff_at < sstream->strbuff_end ?
                sstream->strbuff_at :
                NULL;
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
                sstream->strbuff.capacity);
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
           && str->len < str->capacity)
    {
        str->ptr[str->len] = *curr_char;
        curr_char = gmio_stringstream_next_char(sstream);
        ++str->len;
    }
}

int gmio_get_float32(const char* str, float* value_ptr)
{
#if GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_STD
    char* end_ptr = NULL;
    *value_ptr = gmio_strtof(str, &end_ptr);
    return (end_ptr == str || errno == ERANGE) ? -1 : 0;
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_IRRLICHT
    const char* end_ptr = NULL;
    *value_ptr = fast_strtof(str, &end_ptr);
    return end_ptr == str ? -1 : 0;
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_DOUBLE_CONVERSION
    size_t len = 0;
    while (gmio_ascii_isspace(*str) && *str != '\0')
        ++str;
    while (!gmio_ascii_isspace(*(str + len)) && *(str + len) != '\0')
        ++len;
    *value_ptr = gmio_str2float_googledoubleconversion(str, len);
    return 0;
#endif
}

float gmio_to_float32(const char* str)
{
#if GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_STD
    return gmio_strtof(str, NULL);
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_IRRLICHT
    return fast_atof(str);
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_DOUBLE_CONVERSION
    return gmio_str2float_googledoubleconversion(str, strlen(str));
#endif
}

float gmio_stringstream_parse_float32(struct gmio_stringstream* sstream)
{
#if GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_STD
    char num[32];
    struct gmio_string strnum = { num, 0, sizeof(num) };
    gmio_stringstream_eat_word(sstream, &strnum);
    return (float)atof(strbuff_ptr);
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_IRRLICHT
    return gmio_stringstream_fast_atof(sstream);
#elif GMIO_STR2FLOAT_LIB == GMIO_STR2FLOAT_LIB_DOUBLE_CONVERSION
    char num[32];
    struct gmio_string strnum = gmio_string(num, 0, sizeof(num));
    gmio_stringstream_eat_word(sstream, &strnum);
    return gmio_str2float_googledoubleconversion(num, strnum.len);
#endif
}

#endif /* GMIO_INTERNAL_STRINGSTREAM_H */
