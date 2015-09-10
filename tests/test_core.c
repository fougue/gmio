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

#include "utest_lib.h"

#include "../src/gmio_core/buffer.h"
#include "../src/gmio_core/endian.h"
#include "../src/gmio_core/error.h"
#include "../src/gmio_core/stream.h"

#include <stdlib.h>
#include <string.h>

static gmio_buffer_t buffer_ctor()
{
    return gmio_buffer_calloc(4, 256);
}

const char* test_core__buffer()
{
    /* gmio_buffer_calloc() */
    {
        const size_t obj_count = 4;
        const size_t obj_size = 256;
        const size_t buff_size = obj_count * obj_size;
        const uint8_t zero_buff[4 * 256] = { 0 };
        gmio_buffer_t buff = gmio_buffer_calloc(obj_count, obj_size);
        UTEST_ASSERT(buff.ptr != NULL);
        UTEST_ASSERT(buff.size == buff_size);
        UTEST_ASSERT(memcmp(buff.ptr, &zero_buff[0], buff_size) == 0);
        UTEST_ASSERT(buff.func_deallocate == &free);
        gmio_buffer_deallocate(&buff);
    }
    /* gmio_buffer_malloc() */
    {
        const size_t buff_size = 2 * 1024; /* 2KB */
        gmio_buffer_t buff = gmio_buffer_malloc(buff_size);
        UTEST_ASSERT(buff.ptr != NULL);
        UTEST_ASSERT(buff.size == buff_size);
        UTEST_ASSERT(buff.func_deallocate == &free);
        gmio_buffer_deallocate(&buff);
    }
    /* gmio_buffer_realloc() */
    {
        const size_t buff_size = 1024; /* 1KB */
        gmio_buffer_t buff = gmio_buffer_malloc(buff_size);
        buff = gmio_buffer_realloc(buff.ptr, 2 * buff_size);
        UTEST_ASSERT(buff.ptr != NULL);
        UTEST_ASSERT(buff.size == (2 * buff_size));
        UTEST_ASSERT(buff.func_deallocate == &free);
        gmio_buffer_deallocate(&buff);
    }
    /* default ctor */
    {
        UTEST_ASSERT(gmio_buffer_default_constructor() != NULL);
        gmio_buffer_set_default_constructor(&buffer_ctor);
        UTEST_ASSERT(gmio_buffer_default_constructor() == &buffer_ctor);
    }

    return NULL;
}

const char* test_core__endian()
{
    UTEST_ASSERT(gmio_host_endianness() == GMIO_ENDIANNESS_HOST);
    UTEST_ASSERT(GMIO_ENDIANNESS_LITTLE != GMIO_ENDIANNESS_BIG);
    UTEST_ASSERT(GMIO_ENDIANNESS_HOST == GMIO_ENDIANNESS_LITTLE
                 || GMIO_ENDIANNESS_HOST == GMIO_ENDIANNESS_BIG);
    return NULL;
}

const char* test_core__error()
{
    UTEST_ASSERT(gmio_no_error(GMIO_ERROR_OK));
    UTEST_ASSERT(!gmio_error(GMIO_ERROR_OK));

    UTEST_ASSERT(!gmio_no_error(GMIO_ERROR_UNKNOWN));
    UTEST_ASSERT(gmio_error(GMIO_ERROR_UNKNOWN));

    return NULL;
}

const char* test_core__stream()
{
    {
        const gmio_stream_t null_stream = gmio_stream_null();
        const uint8_t null_bytes[sizeof(gmio_stream_t)] = { 0 };
        UTEST_ASSERT(memcmp(&null_stream, &null_bytes, sizeof(gmio_stream_t))
                     == 0);
    }

    return NULL;
}

const char* all_tests()
{
    UTEST_SUITE_START();
    UTEST_RUN(test_core__buffer);
    UTEST_RUN(test_core__endian);
    UTEST_RUN(test_core__error);
    UTEST_RUN(test_core__stream);
    return NULL;
}

UTEST_MAIN(all_tests)