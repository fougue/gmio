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

/* WARNING :
 *     this header has no multi-inclusion guard. It must be included only once
 *     in the translation unit of use. The reason is that all functions
 *     defined here are meant to be inlined for performance purpose
 */

#include "../stream.h"

/*! Safe and convenient function for gmio_stream::at_end_func() */
GMIO_INLINE static gmio_bool_t gmio_stream_at_end(gmio_stream_t* stream)
{
    if (stream != NULL && stream->at_end_func != NULL)
        return stream->at_end_func(stream->cookie);
    return GMIO_FALSE;
}

/*! Safe and convenient function for gmio_stream::error_func()  */
GMIO_INLINE static int gmio_stream_error(gmio_stream_t* stream)
{
    if (stream != NULL && stream->error_func != NULL)
        return stream->error_func(stream->cookie);
    return 0;
}

/*! Safe and convenient function for gmio_stream::read_func() */
GMIO_INLINE static size_t gmio_stream_read(
        gmio_stream_t* stream, void *ptr, size_t size, size_t count)
{
    if (stream != NULL && stream->read_func != NULL)
        return stream->read_func(stream->cookie, ptr, size, count);
    return 0;
}

/*! Safe and convenient function for gmio_stream::write_func() */
GMIO_INLINE static size_t gmio_stream_write(
        gmio_stream_t* stream, const void *ptr, size_t size, size_t count)
{
    if (stream != NULL && stream->write_func != NULL)
        return stream->write_func(stream->cookie, ptr, size, count);
    return 0;
}

/*! Safe and convenient function for gmio_stream::size_func() */
GMIO_INLINE static size_t gmio_stream_size(gmio_stream_t* stream)
{
    if (stream != NULL && stream->size_func != NULL)
        return stream->size_func(stream->cookie);
    return 0;
}

/*! Safe and convenient function for gmio_stream::rewind_func() */
GMIO_INLINE static void gmio_stream_rewind(gmio_stream_t* stream)
{
    if (stream != NULL && stream->rewind_func != NULL)
        stream->rewind_func(stream->cookie);
}
