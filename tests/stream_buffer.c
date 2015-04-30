/****************************************************************************
** GeomIO Library
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

#include "stream_buffer.h"

#include <string.h>

static gmio_bool_t gmio_stream_buffer_at_end(void* cookie)
{
    const gmio_stream_buffer_t* buff = (const gmio_stream_buffer_t*)cookie;
    return buff->pos >= buff->len;
}

static int gmio_stream_buffer_error(void* cookie)
{
    const gmio_stream_buffer_t* buff = (const gmio_stream_buffer_t*)cookie;
    return buff == NULL || buff->pos > buff->len;
}

static size_t gmio_stream_buffer_read(
        void* cookie, void* ptr, size_t item_size, size_t item_count)
{
    if (item_size > 0 && item_count > 0) {
        gmio_stream_buffer_t* buff = (gmio_stream_buffer_t*)cookie;
        const void* buff_ptr =
                buff->readonly_ptr != NULL ?
                    buff->readonly_ptr : buff->readwrite_ptr;
        const size_t buff_remaining_size = buff->len - buff->pos;
        const size_t wanted_read_size = item_size * item_count;
        const size_t next_read_size =
                wanted_read_size <= buff_remaining_size ?
                    wanted_read_size : buff_remaining_size;
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
        gmio_stream_buffer_t* buff = (gmio_stream_buffer_t*)cookie;
        const size_t buff_remaining_size = buff->len - buff->pos;
        const size_t wanted_write_size = item_size * item_count;
        const size_t next_write_size =
                wanted_write_size <= buff_remaining_size ?
                    wanted_write_size : buff_remaining_size;
        const size_t next_write_item_count = next_write_size / item_size;

        memcpy((char*)buff->readwrite_ptr + buff->pos,
               ptr,
               next_write_item_count * item_size);
        buff->pos += next_write_item_count * item_size;
        return next_write_item_count;
    }
    else {
        return 0;
    }
}

static size_t gmio_stream_buffer_size(void* cookie)
{
    const gmio_stream_buffer_t* buff = (const gmio_stream_buffer_t*)cookie;
    return buff->len;
}

static void gmio_stream_buffer_rewind(void* cookie)
{
    gmio_stream_buffer_t* buff = (gmio_stream_buffer_t*)cookie;
    buff->pos = 0;
}

void gmio_stream_set_buffer(gmio_stream_t *stream, gmio_stream_buffer_t* buff)
{
    stream->cookie = buff;
    stream->at_end_func = gmio_stream_buffer_at_end;
    stream->error_func = gmio_stream_buffer_error;
    stream->read_func = gmio_stream_buffer_read;
    stream->write_func = gmio_stream_buffer_write;
    stream->size_func = gmio_stream_buffer_size;
    stream->rewind_func = gmio_stream_buffer_rewind;
}
