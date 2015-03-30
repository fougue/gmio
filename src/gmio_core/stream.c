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
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include "stream.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

void gmio_stream_set_null(gmio_stream_t* stream)
{
    memset(stream, 0, sizeof(gmio_stream_t));
}

gmio_stream_t gmio_stream_null()
{
    gmio_stream_t null_stream = { 0 };
    return null_stream;
}

static gmio_bool_t gmio_stream_stdio_at_end(void* cookie)
{
    return feof((FILE*) cookie);
}

static int gmio_stream_stdio_error(void* cookie)
{
    return ferror((FILE*) cookie);
}

static size_t gmio_stream_stdio_read(
        void* cookie, void* ptr, size_t item_size, size_t item_count)
{
    return fread(ptr, item_size, item_count, (FILE*) cookie);
}

static size_t gmio_stream_stdio_write(
        void* cookie, const void* ptr, size_t item_size, size_t item_count)
{
    return fwrite(ptr, item_size, item_count, (FILE*) cookie);
}

static size_t gmio_stream_stdio_size(void* cookie)
{
    struct stat stat_buf;
    fstat(fileno((FILE*) cookie), &stat_buf);
    return stat_buf.st_size;
}

static void gmio_stream_stdio_rewind(void* cookie)
{
    rewind((FILE*) cookie);
}

void gmio_stream_set_stdio(gmio_stream_t* stream, FILE* file)
{
    stream->cookie = file;
    stream->at_end_func = gmio_stream_stdio_at_end;
    stream->error_func = gmio_stream_stdio_error;
    stream->read_func = gmio_stream_stdio_read;
    stream->write_func = gmio_stream_stdio_write;
    stream->size_func = gmio_stream_stdio_size;
    stream->rewind_func = gmio_stream_stdio_rewind;
}

gmio_stream_t gmio_stream_stdio(FILE* file)
{
    gmio_stream_t stdio_stream = { 0 };
    gmio_stream_set_stdio(&stdio_stream, file);
    return stdio_stream;
}
