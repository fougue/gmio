/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
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

#ifndef GMIO_INTERNAL_HELPER_STREAM_H
#define GMIO_INTERNAL_HELPER_STREAM_H

#include "../stream.h"

/*! Safe and convenient function for gmio_stream::func_at_end() */
GMIO_INLINE gmio_bool_t gmio_stream_at_end(struct gmio_stream* stream)
{
    if (stream != NULL && stream->func_at_end != NULL)
        return stream->func_at_end(stream->cookie);
    return GMIO_FALSE;
}

/*! Safe and convenient function for gmio_stream::func_error()  */
GMIO_INLINE int gmio_stream_error(struct gmio_stream* stream)
{
    if (stream != NULL && stream->func_error != NULL)
        return stream->func_error(stream->cookie);
    return 0;
}

/*! Safe and convenient function for gmio_stream::func_read() */
GMIO_INLINE size_t gmio_stream_read(
        struct gmio_stream* stream, void *ptr, size_t size, size_t count)
{
    if (stream != NULL && stream->func_read != NULL)
        return stream->func_read(stream->cookie, ptr, size, count);
    return 0;
}

/*! Safe and convenient function for gmio_stream::func_write() */
GMIO_INLINE size_t gmio_stream_write(
        struct gmio_stream* stream, const void *ptr, size_t size, size_t count)
{
    if (stream != NULL && stream->func_write != NULL)
        return stream->func_write(stream->cookie, ptr, size, count);
    return 0;
}

/*! Safe and convenient function for gmio_stream::func_size() */
GMIO_INLINE gmio_streamsize_t gmio_stream_size(struct gmio_stream* stream)
{
    if (stream != NULL && stream->func_size != NULL)
        return stream->func_size(stream->cookie);
    return 0;
}

/*! Safe and convenient function for gmio_stream::func_get_pos() */
GMIO_INLINE int gmio_stream_get_pos(
        struct gmio_stream* stream, struct gmio_streampos* pos)
{
    if (stream != NULL && stream->func_get_pos != NULL)
        return stream->func_get_pos(stream->cookie, pos);
    return -1;
}

/*! Safe and convenient function for gmio_stream::func_set_pos() */
GMIO_INLINE int gmio_stream_set_pos(
        struct gmio_stream* stream, const struct gmio_streampos* pos)
{
    if (stream != NULL && stream->func_set_pos != NULL)
        return stream->func_set_pos(stream->cookie, pos);
    return -1;
}
#endif /* GMIO_INTERNAL_HELPER_STREAM_H */
