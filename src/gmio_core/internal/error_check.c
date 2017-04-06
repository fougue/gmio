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

#include "error_check.h"

#include "locale_utils.h"
#include "../error.h"
#include "../memblock.h"
#include "../stream.h"

bool gmio_check_memblock(int *error, const struct gmio_memblock* mblock)
{
    if (mblock == NULL || mblock->ptr == NULL)
        *error = GMIO_ERROR_NULL_MEMBLOCK;
    else if (mblock->size == 0)
        *error = GMIO_ERROR_INVALID_MEMBLOCK_SIZE;
    return gmio_no_error(*error);
}

bool gmio_check_memblock_size(
        int *error, const struct gmio_memblock *mblock, size_t minsize)
{
    if (gmio_check_memblock(error, mblock) && mblock->size < minsize)
        *error = GMIO_ERROR_INVALID_MEMBLOCK_SIZE;
    return gmio_no_error(*error);
}

bool gmio_check_lc_numeric(int *error)
{
    if (!gmio_lc_numeric_is_C())
        *error = GMIO_ERROR_BAD_LC_NUMERIC;
    return gmio_no_error(*error);
}

bool gmio_check_istream(int *error, const struct gmio_stream *stream)
{
    if (stream == NULL)
        *error = GMIO_ERROR_NULL_STREAM;
    else if (stream->func_at_end == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    else if (stream->func_error == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    else if (stream->func_read == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    else if (stream->func_size == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    else if (stream->func_get_pos == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    else if (stream->func_set_pos == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    return gmio_no_error(*error);
}

bool gmio_check_ostream(int *error, const struct gmio_stream *stream)
{
    if (stream == NULL)
        *error = GMIO_ERROR_NULL_STREAM;
    else if (stream->func_error == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    else if (stream->func_write == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    else if (stream->func_get_pos == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    else if (stream->func_set_pos == NULL)
        *error = GMIO_ERROR_STREAM_FUNC_REQUIRED;
    return gmio_no_error(*error);
}

bool gmio_check_stream(int *error, const struct gmio_stream *stream)
{
    return gmio_check_istream(error, stream)
           && gmio_check_ostream(error, stream);
}
