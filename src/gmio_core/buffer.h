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

/*! \file buffer.h
 *  Declaration of gmio_buffer
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

    /*! Optional pointer on function that deallocates the memory block \p ptr */
    void (*deallocate_func)(void* ptr);
};
typedef struct gmio_buffer gmio_buffer_t;

GMIO_C_LINKAGE_BEGIN

/*! Returns an initialized gmio_buffer object
 *
 *  If \p ptr is NULL then gmio_buffer::size is forced to \c 0
 */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer(void* ptr, size_t size);

/*! Returns a gmio_buffer object allocated with standard malloc() */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer_malloc(size_t size);

/*! Returns a gmio_buffer object allocated with standard calloc() */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer_calloc(size_t num, size_t size);

/*! Returns a gmio_buffer object allocated with OS-specific alloca() */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer_alloca(size_t size);

/*! Safe and convenient call to gmio_buffer::deallocate_func() */
GMIO_LIB_EXPORT void gmio_buffer_deallocate(gmio_buffer_t* buffer);

GMIO_C_LINKAGE_END

#endif /* GMIO_BUFFER_H */
