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

#ifndef GMIO_INTERNAL_STRING_PARSE_H
#define GMIO_INTERNAL_STRING_PARSE_H

#include "../global.h"
#include "../stream.h"
#include "string.h"

/*! Forward iterator over a stream
 *
 *  To be used with API below.
 *  It allows to iterate over a stream (until end is reached) as if it was a
 *  string.
 */
struct gmio_string_stream_fwd_iterator
{
    gmio_stream_t* stream;
    gmio_string_t strbuff;
    const char* strbuff_ptr_end; /*!< Position after last char in strbuff */
    const char* strbuff_ptr_at;  /*!< Position indicator in buffer */

    void* cookie;
    void (*func_stream_read_hook)(
            void* cookie, const gmio_string_t* str_buffer);
};

typedef struct gmio_string_stream_fwd_iterator
        gmio_string_stream_fwd_iterator_t;

/*! Initializes iterator */
void gmio_string_stream_fwd_iterator_init(
        gmio_string_stream_fwd_iterator_t* it);

/*! Returns the char where the iterator is currently pointing at */
GMIO_INLINE const char* gmio_current_char(
        const gmio_string_stream_fwd_iterator_t* it);

/*! Moves on next char in stream */
GMIO_INLINE const char *gmio_next_char(
        gmio_string_stream_fwd_iterator_t *it);

/*! Advances iterator until the first non-space char */
GMIO_INLINE const char* gmio_skip_spaces(
        gmio_string_stream_fwd_iterator_t* it);

/*! Advances iterator until the first non-space char and copies in \p str any
 *  space found */
GMIO_INLINE void gmio_copy_spaces(
        gmio_string_stream_fwd_iterator_t* it,
        gmio_string_t* str);

/*! Error codes returned by gmio_eat_word() */
enum gmio_eat_word_error
{
    GMIO_EAT_WORD_ERROR_OK = 0,
    GMIO_EAT_WORD_ERROR_EMPTY,
    GMIO_EAT_WORD_ERROR_CAPACITY_OVERFLOW
};
typedef enum gmio_eat_word_error gmio_eat_word_error_t;

/*! Advances iterator so that next word is extracted into \p str */
gmio_eat_word_error_t gmio_eat_word(
        gmio_string_stream_fwd_iterator_t* it, gmio_string_t* str);

#if 0
/*! Iterate over stream while it matches input string \p str
 *
 *  Returns GMIO_TRUE if \p str was fully matched
 */
gmio_bool_t gmio_checked_next_chars(
        gmio_string_stream_fwd_iterator_t* it, const char* str);
#endif

/*! Converts the string pointed to by \p str to gmio_float32_t representation
 *
 *  \retval 0  On success
 *  \retval -1 On error(check \c errno to see what happened)
 */
GMIO_INLINE int gmio_get_float32(const char* str, gmio_float32_t* value_ptr);

GMIO_INLINE gmio_float32_t gmio_to_float32(const char* str);


/*
 * -- Implementation
 */

#include "helper_stream.h"
#include "string_utils.h"
#ifdef GMIO_STRINGPARSE_USE_FAST_ATOF
#  include "fast_atof.h"
#endif

#include <errno.h>
#include <stdlib.h>

const char* gmio_current_char(
        const gmio_string_stream_fwd_iterator_t* it)
{
    return it->strbuff_ptr_at < it->strbuff_ptr_end ?
                it->strbuff_ptr_at :
                NULL;
}

const char *gmio_next_char(gmio_string_stream_fwd_iterator_t *it)
{
    ++(it->strbuff_ptr_at);
    if (it->strbuff_ptr_at < it->strbuff_ptr_end)
        return it->strbuff_ptr_at;

    /* Read next chunk of data */
    it->strbuff_ptr_at = it->strbuff.ptr;
    it->strbuff.len = gmio_stream_read(
                it->stream, it->strbuff.ptr, sizeof(char), it->strbuff.max_len);
    it->strbuff_ptr_end = it->strbuff.ptr + it->strbuff.len;
    if (it->strbuff.len > 0) {
        if (it->func_stream_read_hook != NULL)
            it->func_stream_read_hook(it->cookie, &it->strbuff);
        return it->strbuff.ptr;
    }
    return NULL;
}

const char* gmio_skip_spaces(
        gmio_string_stream_fwd_iterator_t* it)
{
    const char* curr_char = gmio_current_char(it);
    while (curr_char != NULL && gmio_ascii_isspace(*curr_char))
        curr_char = gmio_next_char(it);
    return curr_char;
}

void gmio_copy_spaces(
        gmio_string_stream_fwd_iterator_t* it, gmio_string_t* str)
{
    const char* curr_char = gmio_current_char(it);
    while (curr_char != NULL
           && gmio_ascii_isspace(*curr_char)
           && str->len < str->max_len)
    {
        str->ptr[str->len] = *curr_char;
        curr_char = gmio_next_char(it);
        ++str->len;
    }
}

int gmio_get_float32(const char* str, gmio_float32_t* value_ptr)
{
#if defined(GMIO_STRINGPARSE_USE_FAST_ATOF)
    const char* end_ptr = NULL;
    *value_ptr = fast_atof(str, &end_ptr);
#elif defined(GMIO_HAVE_STRTOF_FUNC) /* Requires C99 */
    char* end_ptr = NULL;
    *value_ptr = strtof(str, &end_ptr);
#else
    char* end_ptr = NULL;
    *value_ptr = (gmio_float32_t)strtod(str, &end_ptr);
#endif
    return (end_ptr == str || errno == ERANGE) ? -1 : 0;
}

GMIO_INLINE gmio_float32_t gmio_to_float32(const char* str)
{
#if defined(GMIO_STRINGPARSE_USE_FAST_ATOF)
    return fast_atof(str, NULL);
#elif defined(GMIO_HAVE_STRTOF_FUNC) /* Requires C99 */
    return strtof(str, NULL);
#else
    return (gmio_float32_t)strtod(str, NULL);
#endif
}

#endif /* GMIO_INTERNAL_STRING_PARSE_H */
