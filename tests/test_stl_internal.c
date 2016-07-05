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

#include "utest_assert.h"

#include "stl_utils.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_stl/internal/stl_rw_common.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_io.h"

#include <stddef.h>

static const char* test_stl_internal__rw_common()
{
    /* gmio_check_memblock() */
    {
        int error = GMIO_ERROR_OK;
        uint8_t buff[128] = {0};
        struct gmio_memblock mblock = {0};

        UTEST_ASSERT(!gmio_check_memblock(&error, NULL));
        UTEST_ASSERT(error == GMIO_ERROR_NULL_MEMBLOCK);

        UTEST_ASSERT(!gmio_check_memblock(&error, &mblock));
        UTEST_ASSERT(error == GMIO_ERROR_NULL_MEMBLOCK);

        mblock = gmio_memblock(buff, 0, NULL);
        UTEST_ASSERT(!gmio_check_memblock(&error, &mblock));
        UTEST_ASSERT(error == GMIO_ERROR_INVALID_MEMBLOCK_SIZE);

        /* Verify that gmio_check_memblock() doesn't touch error when in case of
         * success */
        mblock = gmio_memblock(buff, sizeof(buff), NULL);
        UTEST_ASSERT(!gmio_check_memblock(&error, &mblock));
        UTEST_ASSERT(error == GMIO_ERROR_INVALID_MEMBLOCK_SIZE);

        error = GMIO_ERROR_OK;
        UTEST_ASSERT(gmio_check_memblock(&error, &mblock));
        UTEST_ASSERT(error == GMIO_ERROR_OK);
    }

    /* gmio_check_memblock_size() */
    {
        uint8_t buff[1024] = {0};
        struct gmio_memblock mblock =
                gmio_memblock(buff, GMIO_STLB_MIN_CONTENTS_SIZE / 2, NULL);
        int error = GMIO_ERROR_OK;

        UTEST_ASSERT(!gmio_check_memblock_size(
                         &error, &mblock, GMIO_STLB_MIN_CONTENTS_SIZE));
        UTEST_ASSERT(error == GMIO_ERROR_INVALID_MEMBLOCK_SIZE);
    }

    /* gmio_stl_check_mesh() */
    {
        int error = GMIO_ERROR_OK;
        struct gmio_stl_mesh mesh = {0};

        UTEST_ASSERT(!gmio_stl_check_mesh(&error, NULL));
        UTEST_ASSERT(error == GMIO_STL_ERROR_NULL_FUNC_GET_TRIANGLE);

        mesh.triangle_count = 100;
        UTEST_ASSERT(!gmio_stl_check_mesh(&error, &mesh));
        UTEST_ASSERT(error == GMIO_STL_ERROR_NULL_FUNC_GET_TRIANGLE);

        /* Verify that gmio_stl_check_mesh() doesn't touch error when in case of
         * success */
        mesh.triangle_count = 0;
        mesh.func_get_triangle = &gmio_stl_nop_get_triangle;
        UTEST_ASSERT(!gmio_stl_check_mesh(&error, &mesh));
        UTEST_ASSERT(error == GMIO_STL_ERROR_NULL_FUNC_GET_TRIANGLE);

        error = GMIO_ERROR_OK;
        UTEST_ASSERT(gmio_stl_check_mesh(&error, &mesh));
        UTEST_ASSERT(error == GMIO_ERROR_OK);
    }

    /* gmio_stlb_check_byteorder() */
    {
        int error = GMIO_ERROR_OK;

        UTEST_ASSERT(gmio_stlb_check_byteorder(&error, GMIO_ENDIANNESS_HOST));
        UTEST_ASSERT(error == GMIO_ERROR_OK);

        UTEST_ASSERT(!gmio_stlb_check_byteorder(&error, GMIO_ENDIANNESS_UNKNOWN));
        UTEST_ASSERT(error == GMIO_STL_ERROR_UNSUPPORTED_BYTE_ORDER);
    }

    return NULL;
}
