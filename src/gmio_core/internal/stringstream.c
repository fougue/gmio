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

#include "stringstream.h"

#include "helper_stream.h"

struct gmio_stringstream gmio_stringstream(
        const struct gmio_stream stream, const struct gmio_string strbuff)
{
    struct gmio_stringstream sstream = {0};
    sstream.stream = stream;
    sstream.strbuff = strbuff;
    sstream.func_stream_read = gmio_stringstream_default_func_read;
    gmio_stringstream_init_pos(&sstream);
    return sstream;
}

void gmio_stringstream_init_pos(struct gmio_stringstream *sstream)
{
    /* Trick: declaring the buffer exhausted will actually trigger the first
     * call to gmio_stream_read() inside gmio_stringstream_next_char()
     */
    sstream->strbuff.len = 0;
    sstream->strbuff_end = sstream->strbuff.ptr;
    sstream->strbuff_at = sstream->strbuff_end;
    gmio_stringstream_next_char(sstream);
}

size_t gmio_stringstream_default_func_read(
        void* cookie, struct gmio_stream* stream, char* ptr, size_t len)
{
    GMIO_UNUSED(cookie);
    return gmio_stream_read(stream, ptr, 1, len);
}

enum gmio_eat_word_error gmio_stringstream_eat_word(
        struct gmio_stringstream *sstream,
        struct gmio_string *str)
{
    char* str_ptr_at = str->ptr + str->len;
    const char* str_ptr_end = str->ptr + str->capacity;
    const char* stream_curr_char = NULL;

    /* assert(str != NULL && str->ptr != NULL); */

    stream_curr_char = gmio_stringstream_skip_ascii_spaces(sstream);
    if (stream_curr_char == NULL) { /* Empty word */
        *str_ptr_at = 0;
        return GMIO_EAT_WORD_ERROR_EMPTY;
    }

    do {
        *str_ptr_at = *stream_curr_char;
        stream_curr_char = gmio_stringstream_next_char(sstream);
        ++str_ptr_at;
    } while (stream_curr_char != NULL
             && !gmio_ascii_isspace(*stream_curr_char)
             && str_ptr_at < str_ptr_end);

    if (str_ptr_at < str_ptr_end) {
        *str_ptr_at = 0; /* End string with null byte */
        str->len = str_ptr_at - str->ptr;
        return GMIO_EAT_WORD_ERROR_OK;
    }
    return GMIO_EAT_WORD_ERROR_CAPACITY_OVERFLOW;
}

#if 0
bool gmio_stringstream_checked_next_chars(
        struct gmio_stringstream *sstream, const char *str)
{
    size_t pos = 0;
    const char* curr_char = gmio_stringstream_current_char(sstream);
    bool same = curr_char != NULL && *curr_char == *str;

    while (same) {
        curr_char = gmio_stringstream_next_char(sstream);
        same = curr_char != NULL && *curr_char == str[++pos];
    }

    return same;
}
#endif
