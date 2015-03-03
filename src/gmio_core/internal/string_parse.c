/****************************************************************************
**
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
**
****************************************************************************/

#include "string_parse.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

void gmio_string_stream_fwd_iterator_init(gmio_string_stream_fwd_iterator_t *it)
{
    /* Trick: declaring the buffer exhausted will actually trigger the first call to
   * gmio_stream_read() inside gmio_next_char()
   */
    it->buffer.len = 0;
    it->buffer_pos = it->buffer.max_len;
    gmio_next_char(it);
}

char *gmio_current_char(const gmio_string_stream_fwd_iterator_t *it)
{
    if (it != NULL && it->buffer_pos < it->buffer.len)
        return it->buffer.ptr + it->buffer_pos;
    return NULL;
}

char *gmio_next_char(gmio_string_stream_fwd_iterator_t *it)
{
    if (it == NULL)
        return NULL;

    if ((it->buffer_pos + 1) < it->buffer.len) {
        ++(it->buffer_pos);
        return it->buffer.ptr + it->buffer_pos;
    }
    else {
        if (gmio_stream_error(it->stream) != 0 || gmio_stream_at_end(it->stream))
            return NULL;

        /* Read next chunk of data */
        it->buffer_pos = 0;
        it->buffer.len = gmio_stream_read(it->stream, it->buffer.ptr, sizeof(char), it->buffer.max_len);
        if (gmio_stream_error(it->stream) == 0) {
            if (it->stream_read_hook != NULL)
                it->stream_read_hook(it->cookie, &it->buffer);
            return it->buffer.ptr;
        }
    }

    return NULL;
}

void gmio_skip_spaces(gmio_string_stream_fwd_iterator_t *it)
{
    const char* curr_char = gmio_current_char(it);
    while (curr_char != NULL && isspace(*curr_char))
        curr_char = gmio_next_char(it);
}

int gmio_eat_word(gmio_string_stream_fwd_iterator_t *it, gmio_string_buffer_t *buffer)
{
    const char* stream_curr_char = NULL;
    int isspace_res = 0;
    size_t i = 0;

    if (buffer == NULL || buffer->ptr == NULL)
        return -1;

    buffer->len = 0;
    gmio_skip_spaces(it);
    stream_curr_char = gmio_current_char(it);

    while (i < buffer->max_len && stream_curr_char != NULL && isspace_res == 0) {
        isspace_res = isspace(*stream_curr_char);
        if (isspace_res == 0) {
            buffer->ptr[i] = *stream_curr_char;
            stream_curr_char = gmio_next_char(it);
            ++i;
        }
    }

    if (i < buffer->max_len) {
        buffer->ptr[i] = 0; /* End string with terminating null byte */
        buffer->len = i;
        if (stream_curr_char != NULL || gmio_stream_at_end(it->stream))
            return 0;
        return -2;
    }
    return -3;
}

int gmio_get_real32(const char *str, gmio_real32_t *value_ptr)
{
    char* end_ptr; /* for strtod() */

#ifdef GMIO_HAVE_STRTOF_FUNC
    *value_ptr = strtof(str, &end_ptr); /* Requires C99 */
#else
    *value_ptr = (gmio_real32_t)strtod(str, &end_ptr);
#endif

    if (end_ptr == str || errno == ERANGE)
        return -1;

    return 0;
}

gmio_bool_t gmio_checked_next_chars(gmio_string_stream_fwd_iterator_t *it, const char *str)
{
    size_t pos = 0;
    const char* curr_char = gmio_current_char(it);
    gmio_bool_t same = curr_char != NULL && *curr_char == *str;

    while (same) {
        curr_char = gmio_next_char(it);
        same = curr_char != NULL && *curr_char == str[++pos];
    }

    return same;
}
