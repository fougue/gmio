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

#include "utest_assert.h"

#include "stl_utils.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_stl/internal/stl_rw_common.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_io.h"

#include <stddef.h>

const char* test_stl_internal__rw_common()
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
