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
** "http://www.cecill.info".
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
};
typedef struct gmio_buffer gmio_buffer_t;

GMIO_C_LINKAGE_BEGIN

/*! Returns an initialized gmio_buffer object */
GMIO_LIB_EXPORT gmio_buffer_t gmio_buffer(void* ptr, size_t size);

GMIO_C_LINKAGE_END

#endif /* GMIO_BUFFER_H */
