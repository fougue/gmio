/****************************************************************************
**
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
**
****************************************************************************/

#ifndef GMIO_STREAM_H
#define GMIO_STREAM_H

#include "global.h"
#include <stdio.h>

GMIO_C_LINKAGE_BEGIN

/*! \brief Stream that can get input from an arbitrary data source or can write
 *         output to an arbitrary data sink.
 *
 *  It can be seen as generalization of the standard \c FILE*, and is pretty
 *  much the same as [custom streams]
 *  (http://www.gnu.org/software/libc/manual/html_mono/libc.html#Custom-Streams)
 *  in the GNU C Library
 *
 *  It uses a cookie being basically an opaque pointer on a hidden data type.\n
 *  The custom stream is implemented by defining hook functions that know how
 *  to read/write the data.
 */
struct gmio_stream
{
    /*! \brief Opaque pointer on the user stream, passed as first argument to
   *         hook functions */
    void* cookie;

    /*! \brief Pointer on a function that checks end-of-stream indicator
   *
   *  Checks whether the end-of-stream indicator associated with stream
   *  pointed by \p cookie is set, returning GMIO_TRUE if is.
   *
   *  The function should behaves like C standard [feof()]
   *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/feof.html)
   */
    gmio_bool_t (*at_end_func)(void* cookie);

    /*! \brief Pointer on a function that checks error indicator
   *
   *  Checks if the error indicator associated with stream pointed by \p cookie
   *  is set, returning a value different from zero if it is.
   *
   *  The function should behaves like C standard [ferror()]
   *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/ferror.html)
   */
    int (*error_func)(void* cookie);

    /*! \brief Pointer on a function that reads block of data from stream
   *
   *  \details Reads an array of \p count elements, each one with a size of \p size
   *  bytes, from the stream pointed by \p cookie and stores them in the block
   *  of memory specified by \p ptr
   *
   *  The function should behaves like C standard [fread()]
   *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/fread.html)
   *
   *  \returns The total number of elements successfully read
   */
    size_t (*read_func)(void* cookie, void* ptr, size_t size, size_t count);

    /*! \brief Pointer on a function that writes block of data to stream
   *
   *  \details Writes an array of \p count elements, each one with a size of \p size
   *  bytes, from the block of memory pointed by \p ptr to the current position
   *  in the stream pointed by \p cookie
   *
   *  The function should behaves like C standard [fwrite()]
   *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/fwrite.html)
   *
   *  \returns The total number of elements successfully written
   */
    size_t (*write_func)(void* cookie, const void* ptr, size_t size, size_t count);
};

typedef struct gmio_stream gmio_stream_t;

/* Initialization */

/*! \brief Installs a null stream */
GMIO_LIB_EXPORT void gmio_stream_set_null(gmio_stream_t* stream);

/*! \brief Configures \p stream for standard FILE* (cookie will hold \p file) */
GMIO_LIB_EXPORT void gmio_stream_set_stdio(gmio_stream_t* stream, FILE* file);

/* Services */

/*! \brief Safe and convenient function for gmio_stream::at_end_func()
 *
 *  Same as: \code stream->at_end_func(stream->cookie) \endcode
 */
GMIO_LIB_EXPORT gmio_bool_t gmio_stream_at_end(gmio_stream_t* stream);

/*! \brief Safe and convenient function for gmio_stream::error_func()
 *
 *  Same as: \code stream->error_func(stream->cookie) \endcode
 */
GMIO_LIB_EXPORT int gmio_stream_error(gmio_stream_t* stream);

/*! \brief Safe and convenient function for gmio_stream::read_func()
 *
 *  Same as: \code stream->read_func(stream->cookie) \endcode
 */
GMIO_LIB_EXPORT size_t gmio_stream_read(gmio_stream_t* stream,
                                        void* ptr,
                                        size_t size,
                                        size_t count);

/*! \brief Safe and convenient function for gmio_stream::write_func()
 *
 *  Same as: \code stream->write_func(stream->cookie) \endcode
 */
GMIO_LIB_EXPORT size_t gmio_stream_write(gmio_stream_t* stream,
                                         const void* ptr,
                                         size_t size,
                                         size_t count);

GMIO_C_LINKAGE_END

#endif /* GMIO_STREAM_H */
