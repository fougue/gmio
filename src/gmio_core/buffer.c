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

#include "buffer.h"

#include <stdlib.h>

#if defined(GMIO_HAVE_BSD_ALLOCA_FUNC)
#  include <alloca.h>
#elif defined(GMIO_HAVE_WIN_ALLOCA_FUNC)
#  include "error.h"
#  include <windows.h>
#  include <malloc.h>
#endif

GMIO_INLINE static gmio_buffer_t gmio_buffer_null()
{
    gmio_buffer_t buff = { 0 };
    return buff;
}

gmio_buffer_t gmio_buffer(void* ptr, size_t size)
{
    gmio_buffer_t buff;
    buff.ptr = ptr;
    buff.size = size;
    buff.deallocate_func = NULL;
    return buff;
}

gmio_buffer_t gmio_buffer_malloc(size_t size)
{
    gmio_buffer_t buff = gmio_buffer(malloc(size), size);
    buff.deallocate_func = &free;
    return buff;
}

gmio_buffer_t gmio_buffer_calloc(size_t num, size_t size)
{
    gmio_buffer_t buff = gmio_buffer(calloc(num, size), num * size);
    buff.deallocate_func = &free;
    return buff;
}

gmio_buffer_t gmio_buffer_alloca(size_t size)
{
#if defined(GMIO_HAVE_BSD_ALLOCA_FUNC)
    return gmio_buffer(alloca(size), size);
#elif defined(GMIO_HAVE_WIN_ALLOCA_FUNC)
    __try {
        return gmio_buffer(_alloca(size), size);
    }
    __except(GetExceptionCode() == STATUS_STACK_OVERFLOW) {
        /* The stack overflowed */
        if (_resetstkoflw() == 0)
            exit(GMIO_UNKNOWN_ERROR);
        return gmio_buffer_null();
    }
#else
    return gmio_buffer_null();
#endif
}

void gmio_buffer_deallocate(gmio_buffer_t *buffer)
{
    if (buffer != NULL && buffer->deallocate_func != NULL)
        buffer->deallocate_func(buffer->ptr);
}
