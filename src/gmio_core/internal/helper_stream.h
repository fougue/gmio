/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
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

#ifndef GMIO_INTERNAL_HELPER_STREAM_H
#define GMIO_INTERNAL_HELPER_STREAM_H

#include "../stream.h"

/*! Safe and convenient function for gmio_stream::func_at_end() */
GMIO_INLINE bool gmio_stream_at_end(struct gmio_stream* stream);

/*! Safe and convenient function for gmio_stream::func_error() */
GMIO_INLINE int gmio_stream_error(struct gmio_stream* stream);

/*! Safe and convenient function for gmio_stream::func_read() */
GMIO_INLINE size_t gmio_stream_read(
        struct gmio_stream* stream, void *ptr, size_t size, size_t count);

GMIO_INLINE size_t gmio_stream_read_bytes(
        struct gmio_stream* stream, void *ptr, size_t count);

/*! Safe and convenient function for gmio_stream::func_write() */
GMIO_INLINE size_t gmio_stream_write(
        struct gmio_stream* stream, const void *ptr, size_t size, size_t count);

GMIO_INLINE size_t gmio_stream_write_bytes(
        struct gmio_stream* stream, const void *ptr, size_t count);

/*! Safe and convenient function for gmio_stream::func_size() */
GMIO_INLINE gmio_streamsize_t gmio_stream_size(struct gmio_stream* stream);

/*! Safe and convenient function for gmio_stream::func_get_pos() */
GMIO_INLINE int gmio_stream_get_pos(
        struct gmio_stream* stream, struct gmio_streampos* pos);

/*! Returns the current pos object of \p stream */
GMIO_INLINE struct gmio_streampos gmio_streampos(
        struct gmio_stream* stream, int* error);

/*! Safe and convenient function for gmio_stream::func_set_pos() */
GMIO_INLINE int gmio_stream_set_pos(
        struct gmio_stream* stream, const struct gmio_streampos* pos);



/*
 * Implementation
 */

bool gmio_stream_at_end(struct gmio_stream* stream)
{
    if (stream != NULL && stream->func_at_end != NULL)
        return stream->func_at_end(stream->cookie);
    return false;
}

int gmio_stream_error(struct gmio_stream* stream)
{
    if (stream != NULL && stream->func_error != NULL)
        return stream->func_error(stream->cookie);
    return 0;
}

size_t gmio_stream_read(
        struct gmio_stream* stream, void *ptr, size_t size, size_t count)
{
    if (stream != NULL && stream->func_read != NULL)
        return stream->func_read(stream->cookie, ptr, size, count);
    return 0;
}

size_t gmio_stream_read_bytes(
        struct gmio_stream* stream, void *ptr, size_t count)
{
    if (stream != NULL && stream->func_read != NULL)
        return stream->func_read(stream->cookie, ptr, 1, count);
    return 0;
}

size_t gmio_stream_write(
        struct gmio_stream* stream, const void *ptr, size_t size, size_t count)
{
    if (stream != NULL && stream->func_write != NULL)
        return stream->func_write(stream->cookie, ptr, size, count);
    return 0;
}

size_t gmio_stream_write_bytes(
        struct gmio_stream* stream, const void *ptr, size_t count)
{
    if (stream != NULL && stream->func_write != NULL)
        return stream->func_write(stream->cookie, ptr, 1, count);
    return 0;
}

gmio_streamsize_t gmio_stream_size(struct gmio_stream* stream)
{
    if (stream != NULL && stream->func_size != NULL)
        return stream->func_size(stream->cookie);
    return 0;
}

int gmio_stream_get_pos(
        struct gmio_stream* stream, struct gmio_streampos* pos)
{
    if (stream != NULL && stream->func_get_pos != NULL)
        return stream->func_get_pos(stream->cookie, pos);
    return -1;
}

struct gmio_streampos gmio_streampos(struct gmio_stream* stream, int* error)
{
    struct gmio_streampos pos = {0};
    if (stream != NULL && stream->func_get_pos != NULL) {
        const int errcode = stream->func_get_pos(stream->cookie, &pos);
        if (error != NULL)
            *error = errcode;
    }
    return pos;
}

int gmio_stream_set_pos(
        struct gmio_stream* stream, const struct gmio_streampos* pos)
{
    if (stream != NULL && stream->func_set_pos != NULL)
        return stream->func_set_pos(stream->cookie, pos);
    return -1;
}

#endif /* GMIO_INTERNAL_HELPER_STREAM_H */
