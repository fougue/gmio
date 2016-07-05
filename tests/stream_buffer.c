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

#include "stream_buffer.h"

#include <string.h>

static bool gmio_stream_buffer_at_end(void* cookie)
{
    const struct gmio_ro_buffer* buff = (const struct gmio_ro_buffer*)cookie;
    return buff->pos >= buff->len;
}

static int gmio_stream_buffer_error(void* cookie)
{
    const struct gmio_ro_buffer* buff = (const struct gmio_ro_buffer*)cookie;
    return buff == NULL || buff->pos > buff->len;
}

static size_t gmio_stream_buffer_read(
        void* cookie, void* ptr, size_t item_size, size_t item_count)
{
    if (item_size > 0 && item_count > 0) {
        struct gmio_ro_buffer* buff = (struct gmio_ro_buffer*)cookie;
        const void* buff_ptr = buff->ptr;
        const size_t buff_remaining_size = buff->len - buff->pos;
        const size_t wanted_read_size = item_size * item_count;
        const size_t next_read_size =
                wanted_read_size <= buff_remaining_size ?
                    wanted_read_size :
                    buff_remaining_size;
        const size_t next_read_item_count = next_read_size / item_size;

        memcpy(ptr,
               (const char*)buff_ptr + buff->pos,
               next_read_item_count * item_size);
        buff->pos += next_read_item_count * item_size;
        return next_read_item_count;
    }
    else {
        return 0;
    }
}

static size_t gmio_stream_buffer_write(
        void* cookie, const void* ptr, size_t item_size, size_t item_count)
{
    if (item_size > 0 && item_count > 0) {
        struct gmio_rw_buffer* buff = (struct gmio_rw_buffer*)cookie;
        const size_t buff_remaining_size = buff->len - buff->pos;
        const size_t wanted_write_size = item_size * item_count;
        const size_t next_write_size =
                wanted_write_size <= buff_remaining_size ?
                    wanted_write_size :
                    buff_remaining_size;
        const size_t next_write_item_count = next_write_size / item_size;

        memcpy((char*)buff->ptr + buff->pos,
               ptr,
               next_write_item_count * item_size);
        buff->pos += next_write_item_count * item_size;
        return next_write_item_count;
    }
    else {
        return 0;
    }
}

static gmio_streamsize_t gmio_stream_buffer_size(void* cookie)
{
    const struct gmio_ro_buffer* buff = (const struct gmio_ro_buffer*)cookie;
    return buff->len;
}

static int gmio_stream_buffer_get_pos(void* cookie, struct gmio_streampos* pos)
{
    struct gmio_ro_buffer* buff = (struct gmio_ro_buffer*)cookie;
    memcpy(&pos->cookie[0], &buff->pos, sizeof(size_t));
    return 0;
}

static int gmio_stream_buffer_set_pos(
        void* cookie, const struct gmio_streampos* pos)
{
    struct gmio_ro_buffer* buff = (struct gmio_ro_buffer*)cookie;
    memcpy(&buff->pos, &pos->cookie[0], sizeof(size_t));
    return 0;
}

struct gmio_stream gmio_istream_buffer(struct gmio_ro_buffer* buff)
{
    struct gmio_stream stream = {0};
    stream.cookie = buff;
    stream.func_at_end = gmio_stream_buffer_at_end;
    stream.func_error = gmio_stream_buffer_error;
    stream.func_read = gmio_stream_buffer_read;
    stream.func_size = gmio_stream_buffer_size;
    stream.func_get_pos = gmio_stream_buffer_get_pos;
    stream.func_set_pos = gmio_stream_buffer_set_pos;
    return stream;
}

struct gmio_stream gmio_stream_buffer(struct gmio_rw_buffer* buff)
{
    struct gmio_stream stream =
            gmio_istream_buffer((struct gmio_ro_buffer*)buff);
    stream.func_write = gmio_stream_buffer_write;
    return stream;
}

struct gmio_ro_buffer gmio_ro_buffer(const void *ptr, size_t len, size_t pos)
{
    struct gmio_ro_buffer buff = {0};
    buff.ptr = ptr;
    buff.len = len;
    buff.pos = pos;
    return buff;
}

struct gmio_rw_buffer gmio_rw_buffer(void *ptr, size_t len, size_t pos)
{
    struct gmio_rw_buffer buff = {0};
    buff.ptr = ptr;
    buff.len = len;
    buff.pos = pos;
    return buff;
}
