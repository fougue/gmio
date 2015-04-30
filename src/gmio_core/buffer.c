/****************************************************************************
** GeomIO Library
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

#include "buffer.h"

#include <stdlib.h>

GMIO_INLINE gmio_buffer_t gmio_buffer_null()
{
    gmio_buffer_t buff = { 0 };
    return buff;
}

gmio_buffer_t gmio_buffer(
        void* ptr, size_t size, void (*deallocate_func)(void*))
{
    gmio_buffer_t buff;
    buff.ptr = ptr;
    buff.size = ptr != NULL ? size : 0;
    buff.deallocate_func = deallocate_func;
    return buff;
}

gmio_buffer_t gmio_buffer_malloc(size_t size)
{
    return gmio_buffer(malloc(size), size, &free);
}

gmio_buffer_t gmio_buffer_calloc(size_t num, size_t size)
{
    return gmio_buffer(calloc(num, size), num * size, &free);
}

gmio_buffer_t gmio_buffer_realloc(void* ptr, size_t size)
{
    return gmio_buffer(realloc(ptr, size), size, &free);
}

void gmio_buffer_deallocate(gmio_buffer_t *buffer)
{
    if (buffer != NULL && buffer->deallocate_func != NULL)
        buffer->deallocate_func(buffer->ptr);
}

static gmio_buffer_t gmio_buffer_default_internal_ctor()
{
    return gmio_buffer_malloc(128 * 1024); /* 128 KB */
}

/* Warning: global variable ... */
static gmio_buffer_constructor_func_t gmio_global_buffer_ctor =
        gmio_buffer_default_internal_ctor;

void gmio_buffer_set_default_constructor(gmio_buffer_constructor_func_t ctor)
{
    if (ctor != NULL)
        gmio_global_buffer_ctor = ctor;
}

gmio_buffer_constructor_func_t gmio_buffer_default_constructor()
{
    return gmio_global_buffer_ctor;
}

gmio_buffer_t gmio_buffer_default()
{
    return gmio_global_buffer_ctor();
}
