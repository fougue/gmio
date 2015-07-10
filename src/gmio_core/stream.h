/****************************************************************************
** gmio
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

/*! \file stream.h
 *  Declaration of gmio_stream and utility functions
 */

#ifndef GMIO_STREAM_H
#define GMIO_STREAM_H

#include "global.h"
#include <stdio.h>

/*! Stream that can get input from an arbitrary data source or can write
 *  output to an arbitrary data sink.
 *
 *  It can be seen as generalization of the standard \c FILE*, and is pretty
 *  much the same as [custom streams]
 *  (http://www.gnu.org/software/libc/manual/html_mono/libc.html#Custom-Streams)
 *  in the GNU C Library
 *
 *  It uses a cookie being basically an opaque pointer on a hidden data type.
 *
 *  The custom stream is implemented by defining hook functions that know how
 *  to read/write the data.
 */
struct gmio_stream
{
    /*! Opaque pointer on the user stream, passed as first argument to
     *  hook functions */
    void* cookie;

    /*! Pointer on a function that checks end-of-stream indicator
     *
     *  Checks whether the end-of-stream indicator associated with stream
     *  pointed by \p cookie is set, returning GMIO_TRUE if is.
     *
     *  The function should behaves like C standard [feof()]
     *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/feof.html)
     */
    gmio_bool_t (*func_at_end)(void* cookie);

    /*! Pointer on a function that checks error indicator
     *
     *  Checks if the error indicator associated with stream pointed by
     *  \p cookie is set, returning a value different from zero if it is.
     *
     *  The function should behaves like C standard [ferror()]
     *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/ferror.html)
     */
    int (*func_error)(void* cookie);

    /*! Pointer on a function that reads block of data from stream
     *
     *  Reads an array of \p count elements, each one with a size of
     *  \p size bytes, from the stream pointed by \p cookie and stores them in
     *  the block of memory specified by \p ptr
     *
     *  The function should behaves like C standard [fread()]
     *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/fread.html)
     *
     *  \returns The total number of elements successfully read
     */
    size_t (*func_read)(void* cookie, void* ptr, size_t size, size_t count);

    /*! Pointer on a function that writes block of data to stream
     *
     *  Writes an array of \p count elements, each one with a size of
     *  \p size bytes, from the block of memory pointed by \p ptr to the current
     *  position in the stream pointed by \p cookie
     *
     *  The function should behaves like C standard [fwrite()]
     *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/fwrite.html)
     *
     *  \returns The total number of elements successfully written
     */
    size_t (*func_write)(void* cookie, const void* ptr, size_t size, size_t count);

    /*! Pointer on a function that returns the size(in bytes) of the stream */
    size_t (*func_size)(void* cookie);

    /*! Pointer on a function that moves the position indicator within the
     *  stream to the beginning
     *
     *  The function should behaves like C standard [rewind()]
     *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/rewind.html)
     */
    void (*func_rewind)(void* cookie);
};

typedef struct gmio_stream gmio_stream_t;

GMIO_C_LINKAGE_BEGIN

/* Initialization */

/*! Returns a null stream */
GMIO_LIB_EXPORT gmio_stream_t gmio_stream_null();

/*! Returns a stream for standard FILE* (cookie will hold \p file) */
GMIO_LIB_EXPORT gmio_stream_t gmio_stream_stdio(FILE* file);

GMIO_C_LINKAGE_END

#endif /* GMIO_STREAM_H */
