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

/*! \file buffer.h
 *  Declaration of gmio_buffer and utility functions
 */

#ifndef GMIO_BUFFER_H
#define GMIO_BUFFER_H

#include "global.h"

#include <stddef.h>

/*! Basic block of memory */
struct gmio_buffer
{
    /*! Pointer to the beginning of the block of memory */
    void* ptr;

    /*! Size (in bytes) of the memory buffer */
    size_t size;

    /*! Optional pointer on a function that deallocates the memory block
     *  beginning at \p ptr */
    void (*deallocate_func)(void* ptr);
};
typedef struct gmio_buffer gmio_buffer_t;

GMIO_C_LINKAGE_BEGIN

/*! Returns an initialized gmio_buffer object
 *
 *  If \p ptr is NULL then gmio_buffer::size is forced to \c 0
 */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer(
        void* ptr, size_t size, void (*deallocate_func)(void*));

/*! Returns a gmio_buffer object allocated with standard \c malloc() */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer_malloc(size_t size);

/*! Returns a gmio_buffer object allocated with standard \c calloc() */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer_calloc(size_t num, size_t size);

/*! Returns a gmio_buffer object allocated with standard \c realloc() */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer_realloc(void* ptr, size_t size);

/*! Safe and convenient call to gmio_buffer::deallocate_func() */
GMIO_LIB_EXPORT void gmio_buffer_deallocate(gmio_buffer_t* buffer);

/*! Typedef for a pointer to a function that creates an allocated buffer
 *
 *  Signature:
 *  \code gmio_buffer_t buffer_ctor(); \endcode
 */
typedef gmio_buffer_t (*gmio_buffer_constructor_func_t)();

/*! Installs a global function to construct gmio_buffer objects
 *
 *  The constructor function allocates a gmio_buffer object on demand, to be
 *  used when a temporary buffer is needed.
 *
 *  This function is not thread-safe.
 */
GMIO_LIB_EXPORT void gmio_buffer_set_default_constructor(
        gmio_buffer_constructor_func_t ctor);

/*! Returns the currently installed function to construct gmio_buffer objects
 *
 *  It is initialized to <tt>gmio_buffer_malloc(128KB)</tt>
 */
GMIO_LIB_EXPORT gmio_buffer_constructor_func_t gmio_buffer_default_constructor();

/*! Returns a gmio_buffer object created using the function
 *  gmio_buffer_default_constructor() */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer_default();

GMIO_C_LINKAGE_END

#endif /* GMIO_BUFFER_H */
