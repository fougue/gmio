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

/*! \file stream.h
 *  Declaration of gmio_stream and utility functions
 *
 *  \addtogroup gmio_core
 *  @{
 */

#pragma once

#include "global.h"
#include "memblock.h"
#include "streampos.h"
#include <stdio.h>

#ifndef DOXYGEN
#  ifdef GMIO_HAVE_INT64_TYPE
typedef int64_t gmio_streamsize_t;
typedef int64_t gmio_streamoffset_t;
#  else
typedef long gmio_streamsize_t;
typedef long gmio_streamoffset_t;
#  endif
#else
/*! Type able to represent the size(in bytes) of a stream
 *
 *  It can be int64_t or long depending on the compiler support
 */
typedef int64_or_long gmio_streamsize_t;
/*! Type able to represent the offset position within a stream
 *
 *  It can be int64_t or long depending on the compiler support
 */
typedef int64_or_long gmio_streamoffset_t;
#endif

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
     *  pointed by \p cookie is set, returning true if it is.
     *
     *  The function should behaves like C standard [feof()]
     *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/feof.html)
     */
    bool (*func_at_end)(void* cookie);

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
    gmio_streamsize_t (*func_size)(void* cookie);

    /*! Pointer on a function that retrieves the current position in the stream
     *
     *  \retval 0 on success
     *  \retval !=0 on error
     */
    int (*func_get_pos)(void* cookie, struct gmio_streampos* pos);

    /*! Pointer on a function that restores the current position in the stream
     *  to \p pos
     *
     *  \retval 0 on success
     *  \retval !=0 on error
     */
    int (*func_set_pos)(void* cookie, const struct gmio_streampos* pos);
};


GMIO_C_LINKAGE_BEGIN

/* Initialization */

/*! Returns a null stream */
GMIO_API struct gmio_stream gmio_stream_null();

/*! Returns a stream for standard FILE* (cookie will hold \p file) */
GMIO_API struct gmio_stream gmio_stream_stdio(FILE* file);

GMIO_C_LINKAGE_END

/*! @} */
