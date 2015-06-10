/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougsys.fr
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

/* For implementation section */
#include "helper_stream.h"
#include "string_utils.h"
#ifdef GMIO_USE_FAST_ATOF
#  include "fast_atof.h"
#endif

#include <errno.h>
#include <stdlib.h>
/* End for implementation section */

/*! Stores traditional 8-bit strings
 *
 *  For faster length lookups, it knowns the length of its contents. It must
 *  never exceeds the maximum length of the buffer.
 */
struct gmio_string_buffer
{
    char*  ptr; /*!< Buffer contents */
    size_t len; /*!< Size(length) of current contents */
    size_t max_len; /*!< Maximum contents size(length) */
};

typedef struct gmio_string_buffer  gmio_string_buffer_t;

/*! Forward iterator over a stream
 *
 *  To be used with API below.
 *  It allows to iterate over a stream (until end is reached) as if it was a
 *  string.
 */
struct gmio_string_stream_fwd_iterator
{
    gmio_stream_t* stream;
    gmio_string_buffer_t buffer;
    size_t buffer_pos; /*!< Position indicator in buffer */

    void* cookie;
    void (*stream_read_hook)(
            void* cookie, const gmio_string_buffer_t* str_buffer);
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

/*! Advances iterator until the next non-space char */
GMIO_INLINE const char* gmio_skip_spaces(
        gmio_string_stream_fwd_iterator_t* it);

/*! Advances iterator so that next word is extracted into \p buffer
 * 
 *  \retval 0    On success
 *  \retval <=-1 On error
 */
int gmio_eat_word(
        gmio_string_stream_fwd_iterator_t* it, gmio_string_buffer_t* buffer);

/*! Iterate over stream while it matches input string \p str
 *
 *  Returns GMIO_TRUE if \p str was fully matched
 */
gmio_bool_t gmio_checked_next_chars(
        gmio_string_stream_fwd_iterator_t* it, const char* str);

/*! Converts the string pointed to by \p str to gmio_float32_t representation
 *
 *  \retval 0  On success
 *  \retval -1 On error(check \c errno to see what happened)
 */
GMIO_INLINE int gmio_get_float32(const char* str, gmio_float32_t* value_ptr);



/*
 * -- Implementation
 */

const char* gmio_current_char(
        const gmio_string_stream_fwd_iterator_t* it)
{
      if (it->buffer_pos < it->buffer.len)
        return it->buffer.ptr + it->buffer_pos;
    return NULL;
}

const char *gmio_next_char(gmio_string_stream_fwd_iterator_t *it)
{
    ++(it->buffer_pos);
    if (it->buffer_pos < it->buffer.len)
        return it->buffer.ptr + it->buffer_pos;
    
    /* Read next chunk of data */
    it->buffer_pos = 0;
    it->buffer.len = gmio_stream_read(
                it->stream, it->buffer.ptr, sizeof(char), it->buffer.max_len);
    if (it->buffer.len > 0) {
        if (it->stream_read_hook != NULL)
            it->stream_read_hook(it->cookie, &it->buffer);
        return it->buffer.ptr;
    }
    return NULL;
}

const char* gmio_skip_spaces(
        gmio_string_stream_fwd_iterator_t* it)
{
    const char* curr_char = gmio_current_char(it);
    while (curr_char != NULL && gmio_clocale_isspace(*curr_char))
        curr_char = gmio_next_char(it);
    return curr_char;
}

int gmio_get_float32(const char* str, gmio_float32_t* value_ptr)
{
#if defined(GMIO_USE_FAST_ATOF)
    const char* end_ptr = NULL;
    *value_ptr = fast_atof(str, &end_ptr);
#elif defined(GMIO_HAVE_STRTOF_FUNC) /* Requires C99 */
    char* end_ptr = NULL;
    *value_ptr = strtof(str, &end_ptr);
#else
    char* end_ptr = NULL;
    *value_ptr = (gmio_float32_t)strtod(str, &end_ptr);
#endif

    if (end_ptr == str || errno == ERANGE)
        return -1;

    return 0;
}

#endif /* GMIO_INTERNAL_STRING_PARSE_H */
