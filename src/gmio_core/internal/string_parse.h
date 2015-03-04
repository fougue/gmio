/****************************************************************************
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
****************************************************************************/

#ifndef GMIO_INTERNAL_STRING_PARSE_H
#define GMIO_INTERNAL_STRING_PARSE_H

#include "../global.h"
#include "../stream.h"

/*! Stores traditional 8-bit strings
 *
 *  For faster length lookups, it knowns the length of its contents. It must
 *  never exceeds the maximum length of the buffer.
 */
struct gmio_string_buffer
{
    char*  ptr;     /*!< Buffer contents */
    size_t len;     /*!< Size(length) of current contents */
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
    gmio_stream_t*       stream;
    gmio_string_buffer_t buffer;
    size_t               buffer_pos; /*!< Position indicator in buffer */

    void* cookie;
    void (*stream_read_hook)(void* cookie, const gmio_string_buffer_t* str_buffer);
};

typedef struct gmio_string_stream_fwd_iterator  gmio_string_stream_fwd_iterator_t;

void  gmio_string_stream_fwd_iterator_init(gmio_string_stream_fwd_iterator_t* it);
const char* gmio_current_char(const gmio_string_stream_fwd_iterator_t* it);
void  gmio_skip_spaces(gmio_string_stream_fwd_iterator_t* it);
int   gmio_eat_word(gmio_string_stream_fwd_iterator_t* it, gmio_string_buffer_t* buffer);

/*! Move on next char in stream */
const char* gmio_next_char(gmio_string_stream_fwd_iterator_t* it);

/*! Iterate over stream while it matches input string \p str
 *
 *  Returns GMIO_TRUE if \p str was fully matched
 */
gmio_bool_t gmio_checked_next_chars(gmio_string_stream_fwd_iterator_t* it, const char* str);

/*! Converts the string pointed to by \p str to gmio_float32_t representation
 *
 *  \retval 0  On success
 *  \retval -1 On error(check \c errno to see what happened)
 */
int gmio_get_float32(const char* str, gmio_float32_t* value_ptr);

#endif /* GMIO_INTERNAL_STRING_PARSE_H */
