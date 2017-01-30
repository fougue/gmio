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

/*! \file memblock.h
 *  Declaration of gmio_memblock and utility functions
 *
 *  \addtogroup gmio_core
 *  @{
 */

#pragma once

#include "global.h"
#include <stddef.h>

/*! Basic memory block
 *
 *  gmio_memblock comes with convenient constructors that binds to
 *  <tt><stdlib.h></tt> allocation functions, like gmio_memblock_malloc(), ...
 *
 *  Binding a memblock to some statically-allocated memory is done through
 *  gmio_memblock() :
 *  \code{.c}
 *      char buff[512] = {0};
 *      struct gmio_memblock blk =
 *                  gmio_memblock(buff, GMIO_ARRAY_SIZE(buff), NULL);
 *  \endcode
 *
 *  Any memblock can be safely freed with gmio_memblock_deallocate()
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

/*! Returns an initialized memblock object.
 *  If \p ptr is NULL then gmio_memblock::size is forced to \c 0
 */
GMIO_API struct gmio_memblock gmio_memblock(
                void* ptr, size_t size, void (*func_deallocate)(void*));

/*! Returns a memblock allocated with standard \c malloc().
 *  gmio_memblock::func_deallocate is set to standard \c free()
 */
GMIO_API struct gmio_memblock gmio_memblock_malloc(size_t size);

/*! Returns a memblock allocated with standard \c calloc().
 *  gmio_memblock::func_deallocate is set to standard \c free()
 */
GMIO_API struct gmio_memblock gmio_memblock_calloc(size_t num, size_t size);

/*! Returns a memblock allocated with standard \c realloc().
 *  gmio_memblock::func_deallocate is set to standard \c free()
 */
GMIO_API struct gmio_memblock gmio_memblock_realloc(void* ptr, size_t size);

/*! Safe and convenient call to gmio_memblock::func_deallocate() */
GMIO_API void gmio_memblock_deallocate(struct gmio_memblock* mblock);

/*! Typedef for a pointer to a function that creates an allocated memblock
 *
 *  Signature:
 *  \code{.c} struct gmio_memblock mblock_ctor(); \endcode
 */
typedef struct gmio_memblock (*gmio_memblock_constructor_func_t)();

/*! Installs a global function to construct gmio_memblock objects
 *
 *  The constructor function allocates a memblock on demand, to be used when a
 *  temporary memblock is needed.
 *
 *  This function is not thread-safe.
 */
GMIO_API void gmio_memblock_set_default_constructor(
                gmio_memblock_constructor_func_t ctor);

/*! Returns the currently installed function to construct memblock objects.
 *  It is initialized to <tt>gmio_memblock_malloc(128KB)</tt>
 */
GMIO_API gmio_memblock_constructor_func_t gmio_memblock_default_constructor();

/*! Returns a memblock created with the function
 *  gmio_memblock_default_constructor()
 */
GMIO_API struct gmio_memblock gmio_memblock_default();

GMIO_C_LINKAGE_END

/*! @} */
