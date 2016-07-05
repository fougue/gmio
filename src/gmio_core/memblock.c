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

#include "memblock.h"

#include <stdlib.h>

bool gmio_memblock_isnull(const struct gmio_memblock *mblock)
{
    return mblock == NULL || mblock->ptr == NULL || mblock->size == 0;
}

struct gmio_memblock gmio_memblock(
        void* ptr, size_t size, void (*func_deallocate)(void*))
{
    struct gmio_memblock buff;
    buff.ptr = ptr;
    buff.size = ptr != NULL ? size : 0;
    buff.func_deallocate = func_deallocate;
    return buff;
}

struct gmio_memblock gmio_memblock_malloc(size_t size)
{
    return gmio_memblock(malloc(size), size, &free);
}

struct gmio_memblock gmio_memblock_calloc(size_t num, size_t size)
{
    return gmio_memblock(calloc(num, size), num * size, &free);
}

struct gmio_memblock gmio_memblock_realloc(void* ptr, size_t size)
{
    return gmio_memblock(realloc(ptr, size), size, &free);
}

void gmio_memblock_deallocate(struct gmio_memblock *mblock)
{
    if (mblock != NULL && mblock->func_deallocate != NULL) {
        mblock->func_deallocate(mblock->ptr);
        mblock->ptr = NULL;
        mblock->size = 0;
    }
}

static struct gmio_memblock gmio_memblock_default_internal_ctor()
{
    return gmio_memblock_malloc(128 * 1024); /* 128 KB */
}

/* Warning: global variable ... */
static gmio_memblock_constructor_func_t gmio_global_mblock_ctor =
        gmio_memblock_default_internal_ctor;

void gmio_memblock_set_default_constructor(gmio_memblock_constructor_func_t ctor)
{
    if (ctor != NULL)
        gmio_global_mblock_ctor = ctor;
}

gmio_memblock_constructor_func_t gmio_memblock_default_constructor()
{
    return gmio_global_mblock_ctor;
}

struct gmio_memblock gmio_memblock_default()
{
    return gmio_global_mblock_ctor();
}
