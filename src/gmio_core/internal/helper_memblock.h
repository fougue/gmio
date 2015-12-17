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

#ifndef GMIO_INTERNAL_HELPER_MEMBLOCK_H
#define GMIO_INTERNAL_HELPER_MEMBLOCK_H

#include "../memblock.h"

#include <stddef.h>

struct gmio_memblock_helper
{
    struct gmio_memblock* memblock;
    gmio_bool_t was_allocated;
};

GMIO_INLINE
struct gmio_memblock_helper gmio_memblock_helper(struct gmio_memblock* mblock);

GMIO_INLINE
void gmio_memblock_helper_release(struct gmio_memblock_helper* helper);



/*
 * Implementation
 */

struct gmio_memblock_helper gmio_memblock_helper(struct gmio_memblock* mblock)
{
    struct gmio_memblock_helper helper = {0};
    helper.memblock = mblock;
    if (mblock != NULL && (mblock->ptr == NULL || mblock->size == 0)) {
        *(helper.memblock) = gmio_memblock_default();
        helper.was_allocated = GMIO_TRUE;
    }
    return helper;
}

void gmio_memblock_helper_release(struct gmio_memblock_helper* helper)
{
    if (helper != NULL && helper->was_allocated) {
        gmio_memblock_deallocate(helper->memblock);
        helper->memblock = NULL;
        helper->was_allocated = GMIO_FALSE;
    }
}

#endif /* GMIO_INTERNAL_HELPER_MEMBLOCK_H */