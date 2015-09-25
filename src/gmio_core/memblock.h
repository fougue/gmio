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

/*! \file memblock.h
 *  Declaration of gmio_memblock and utility functions
 *
 *  \addtogroup gmio_core
 *  @{
 */

#ifndef GMIO_MEMBLOCK_H
#define GMIO_MEMBLOCK_H

#include "global.h"

#include <stddef.h>

/*! Basic memory block */
struct gmio_memblock
{
    /*! Pointer to the beginning of the memory block */
    void* ptr;

    /*! Size (in bytes) of the memory block */
    size_t size;

    /*! Optional pointer on a function that deallocates the memory block
     *  beginning at \p ptr */
    void (*func_deallocate)(void* ptr);
};
typedef struct gmio_memblock gmio_memblock_t;

GMIO_C_LINKAGE_BEGIN

/*! Returns an initialized gmio_memblock object
 *
 *  If \p ptr is NULL then gmio_memblock::size is forced to \c 0
 */
GMIO_LIB_EXPORT gmio_memblock_t gmio_memblock(
        void* ptr, size_t size, void (*func_deallocate)(void*));

/*! Returns a gmio_memblock object allocated with standard \c malloc() */
GMIO_LIB_EXPORT gmio_memblock_t gmio_memblock_malloc(size_t size);

/*! Returns a gmio_memblock object allocated with standard \c calloc() */
GMIO_LIB_EXPORT gmio_memblock_t gmio_memblock_calloc(size_t num, size_t size);

/*! Returns a gmio_memblock object allocated with standard \c realloc() */
GMIO_LIB_EXPORT gmio_memblock_t gmio_memblock_realloc(void* ptr, size_t size);

/*! Safe and convenient call to gmio_memblock::func_deallocate() */
GMIO_LIB_EXPORT void gmio_memblock_deallocate(gmio_memblock_t* mblock);

/*! Typedef for a pointer to a function that creates an allocated mblock
 *
 *  Signature:
 *  \code gmio_memblock_t mblock_ctor(); \endcode
 */
typedef gmio_memblock_t (*gmio_memblock_constructor_func_t)();

/*! Installs a global function to construct gmio_memblock objects
 *
 *  The constructor function allocates a gmio_memblock object on demand, to be
 *  used when a temporary mblock is needed.
 *
 *  This function is not thread-safe.
 */
GMIO_LIB_EXPORT void gmio_memblock_set_default_constructor(
        gmio_memblock_constructor_func_t ctor);

/*! Returns the currently installed function to construct gmio_memblock objects
 *
 *  It is initialized to <tt>gmio_memblock_malloc(128KB)</tt>
 */
GMIO_LIB_EXPORT gmio_memblock_constructor_func_t gmio_memblock_default_constructor();

/*! Returns a gmio_memblock object created using the function
 *  gmio_memblock_default_constructor() */
GMIO_LIB_EXPORT gmio_memblock_t gmio_memblock_default();

GMIO_C_LINKAGE_END

#endif /* GMIO_MEMBLOCK_H */
/*! @} */
