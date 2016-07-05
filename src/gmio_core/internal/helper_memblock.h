/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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

#ifndef GMIO_INTERNAL_HELPER_MEMBLOCK_H
#define GMIO_INTERNAL_HELPER_MEMBLOCK_H

#include "../memblock.h"

#include <stddef.h>

struct gmio_memblock_helper
{
    struct gmio_memblock memblock;
    bool was_allocated;
};

GMIO_INLINE
struct gmio_memblock_helper gmio_memblock_helper(
        const struct gmio_memblock* mblock);

GMIO_INLINE
void gmio_memblock_helper_release(struct gmio_memblock_helper* helper);



/*
 * Implementation
 */

struct gmio_memblock_helper gmio_memblock_helper(
        const struct gmio_memblock* mblock)
{
    struct gmio_memblock_helper helper = {0};
    if (gmio_memblock_isnull(mblock)) {
        helper.memblock = gmio_memblock_default();
        helper.was_allocated = true;
    }
    else {
        helper.memblock = *mblock;
    }
    return helper;
}

void gmio_memblock_helper_release(struct gmio_memblock_helper* helper)
{
    if (helper != NULL && helper->was_allocated) {
        const struct gmio_memblock mblock_null = {0};
        gmio_memblock_deallocate(&helper->memblock);
        helper->memblock = mblock_null;
        helper->was_allocated = false;
    }
}

#endif /* GMIO_INTERNAL_HELPER_MEMBLOCK_H */
