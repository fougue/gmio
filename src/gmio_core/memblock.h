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

/*! Basic memory block
 *
 *  gmio_memblock comes with convenient constructors that binds to
 *  <tt><stdlib.h></tt> allocation functions, like gmio_memblock_malloc(), ...
 *
 *  Binding gmio_memblock to some statically-allocated memory is done through
 *  gmio_memblock() :
 *  \code{.c}
 *      char buff[512] = {};
 *      struct gmio_memblock blk =
 *                  gmio_memblock(buff, GMIO_ARRAY_SIZE(buff), NULL);
 *  \endcode
 *
 *  Any gmio_memblock object can be safely freed with gmio_memblock_deallocate()
 */
struct gmio_memblock
{
    /*! Pointer to the beginning of the memory block */
    void* ptr;

    /*! Size (in bytes) of the memory block */
    size_t size;

    /*! Optional pointer on a function that deallocates the memory block
     *  beginning at \p ptr
     *
     *  \sa gmio_memblock_deallocate()
     */
    void (*func_deallocate)(void* ptr);
};

GMIO_C_LINKAGE_BEGIN

/*! Returns true if \p mblock is NULL or points to null/void memory */
GMIO_API bool gmio_memblock_isnull(const struct gmio_memblock* mblock);

/*! Returns an initialized gmio_memblock object
 *
 *  If \p ptr is NULL then gmio_memblock::size is forced to \c 0
 */
GMIO_API struct gmio_memblock gmio_memblock(
                void* ptr, size_t size, void (*func_deallocate)(void*));

/*! Returns a gmio_memblock object allocated with standard \c malloc()
 *
 *  gmio_memblock::func_deallocate is set to standard \c free()
 */
GMIO_API struct gmio_memblock gmio_memblock_malloc(size_t size);

/*! Returns a gmio_memblock object allocated with standard \c calloc()
 *
 *  gmio_memblock::func_deallocate is set to standard \c free()
 */
GMIO_API struct gmio_memblock gmio_memblock_calloc(size_t num, size_t size);

/*! Returns a gmio_memblock object allocated with standard \c realloc()
 *
 *  gmio_memblock::func_deallocate is set to standard \c free()
 */
GMIO_API struct gmio_memblock gmio_memblock_realloc(void* ptr, size_t size);

/*! Safe and convenient call to gmio_memblock::func_deallocate() */
GMIO_API void gmio_memblock_deallocate(struct gmio_memblock* mblock);

/*! Typedef for a pointer to a function that creates an allocated mblock
 *
 *  Signature:
 *  \code{.c} struct gmio_memblock mblock_ctor(); \endcode
 */
typedef struct gmio_memblock (*gmio_memblock_constructor_func_t)();

/*! Installs a global function to construct gmio_memblock objects
 *
 *  The constructor function allocates a gmio_memblock object on demand, to be
 *  used when a temporary memblock is needed.
 *
 *  This function is not thread-safe.
 */
GMIO_API void gmio_memblock_set_default_constructor(
                gmio_memblock_constructor_func_t ctor);

/*! Returns the currently installed function to construct gmio_memblock objects
 *
 *  It is initialized to <tt>gmio_memblock_malloc(128KB)</tt>
 */
GMIO_API gmio_memblock_constructor_func_t gmio_memblock_default_constructor();

/*! Returns a gmio_memblock object created using the function
 *  gmio_memblock_default_constructor() */
GMIO_API struct gmio_memblock gmio_memblock_default();

GMIO_C_LINKAGE_END

#endif /* GMIO_MEMBLOCK_H */
/*! @} */
