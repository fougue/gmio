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

#include "stream.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void gmio_stream_set_null(gmio_stream_t* stream)
{
    memset(stream, 0, sizeof(gmio_stream_t));
}

static gmio_bool_t gmio_stream_stdio_at_end(void* cookie)
{
    return feof((FILE*) cookie);
}

static int gmio_stream_stdio_error(void* cookie)
{
    return ferror((FILE*) cookie);
}

static size_t gmio_stream_stdio_read(void* cookie,
                                     void* ptr,
                                     size_t item_size,
                                     size_t item_count)
{
    return fread(ptr, item_size, item_count, (FILE*) cookie);
}

static size_t gmio_stream_stdio_write(void* cookie,
                                      const void* ptr,
                                      size_t item_size,
                                      size_t item_count)
{
    return fwrite(ptr, item_size, item_count, (FILE*) cookie);
}

void gmio_stream_set_stdio(gmio_stream_t* stream, FILE* file)
{
    stream->cookie = file;
    stream->at_end_func = gmio_stream_stdio_at_end;
    stream->error_func = gmio_stream_stdio_error;
    stream->read_func = gmio_stream_stdio_read;
    stream->write_func = gmio_stream_stdio_write;
}

gmio_bool_t gmio_stream_at_end(gmio_stream_t* stream)
{
    if (stream != NULL && stream->at_end_func != NULL)
        return stream->at_end_func(stream->cookie);
    return GMIO_FALSE;
}

int gmio_stream_error(gmio_stream_t* stream)
{
    if (stream != NULL && stream->error_func != NULL)
        return stream->error_func(stream->cookie);
    return 0;
}

size_t gmio_stream_read(gmio_stream_t* stream, void *ptr, size_t size, size_t count)
{
    if (stream != NULL && stream->read_func != NULL)
        return stream->read_func(stream->cookie, ptr, size, count);
    return 0;
}

size_t gmio_stream_write(gmio_stream_t* stream, const void *ptr, size_t size, size_t count)
{
    if (stream != NULL && stream->write_func != NULL)
        return stream->write_func(stream->cookie, ptr, size, count);
    return 0;
}
