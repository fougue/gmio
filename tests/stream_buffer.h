/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
** contact@fougue.pro
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

#ifndef GMIO_STREAM_BUFFER_H
#define GMIO_STREAM_BUFFER_H

#include "../src/gmio_core/stream.h"

/* Read-only buffer */
struct gmio_ro_buffer
{
    const void* ptr;
    size_t len;
    size_t pos;
};

/* Read/write buffer */
struct gmio_rw_buffer
{
    void* ptr;
    size_t len;
    size_t pos;
};

struct gmio_ro_buffer gmio_ro_buffer(const void* ptr, size_t len, size_t pos);
struct gmio_rw_buffer gmio_rw_buffer(void* ptr, size_t len, size_t pos);

struct gmio_stream gmio_istream_buffer(struct gmio_ro_buffer* buff);
struct gmio_stream gmio_iostream_buffer(struct gmio_rw_buffer* buff);

#endif /* GMIO_STREAM_BUFFER_H */
