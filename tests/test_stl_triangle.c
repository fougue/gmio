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

#include "utest_assert.h"

#include "../src/gmio_core/internal/numeric_utils.h"
#include "../src/gmio_stl/stl_constants.h"
#include "../src/gmio_stl/stl_triangle.h"

#include <stddef.h>
#include <string.h>

/* Disable MSVC warning "conditional expression is constant" */
GMIO_PRAGMA_MSVC_WARNING_PUSH_AND_DISABLE(4127)

static const char* test_stl_coords_packing()
{
    UTEST_COMPARE_UINT(0, offsetof(struct gmio_vec3f, x));
    UTEST_COMPARE_UINT(4, offsetof(struct gmio_vec3f, y));
    UTEST_COMPARE_UINT(8, offsetof(struct gmio_vec3f, z));
    UTEST_COMPARE_UINT(GMIO_STL_COORDS_RAWSIZE, sizeof(struct gmio_vec3f));
    return NULL;
}

static const char* test_stl_triangle_packing()
{
    UTEST_COMPARE_UINT(0, offsetof(struct gmio_stl_triangle, n));
    UTEST_COMPARE_UINT(GMIO_STL_COORDS_RAWSIZE, offsetof(struct gmio_stl_triangle, v1));
    UTEST_COMPARE_UINT(2*GMIO_STL_COORDS_RAWSIZE, offsetof(struct gmio_stl_triangle, v2));
    UTEST_COMPARE_UINT(3*GMIO_STL_COORDS_RAWSIZE, offsetof(struct gmio_stl_triangle, v3));
    UTEST_COMPARE_UINT(4*GMIO_STL_COORDS_RAWSIZE, offsetof(struct gmio_stl_triangle, attribute_byte_count));
    UTEST_ASSERT(sizeof(struct gmio_stl_triangle) >= GMIO_STLB_TRIANGLE_RAWSIZE);
    return NULL;
}

static const char* test_stl_triangle_compute_normal()
{
    const unsigned udiff = 5;
    { /* Doesn't fail on invalid facet */
        struct gmio_stl_triangle tri = {0};
        gmio_stl_triangle_compute_normal(&tri);
        UTEST_ASSERT(gmio_float32_ulp_equals(tri.n.x, 0.f, udiff));
        UTEST_ASSERT(gmio_float32_ulp_equals(tri.n.y, 0.f, udiff));
        UTEST_ASSERT(gmio_float32_ulp_equals(tri.n.z, 0.f, udiff));
    }
    return NULL;
}

GMIO_PRAGMA_MSVC_WARNING_POP()
