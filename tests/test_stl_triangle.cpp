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

#include <cstddef>

// Disable MSVC warning "conditional expression is constant"
GMIO_PRAGMA_MSVC_WARNING_PUSH_AND_DISABLE(4127)

namespace gmio {

static const char* TestSTL_triangleCoordsPacking()
{
    UTEST_COMPARE(0, offsetof(Vec3f, x));
    UTEST_COMPARE(4, offsetof(Vec3f, y));
    UTEST_COMPARE(8, offsetof(Vec3f, z));
    UTEST_COMPARE(STL_CoordsRawSize, sizeof(Vec3f));
    return nullptr;
}

static const char* TestSTL_trianglePacking()
{
    UTEST_COMPARE(0, offsetof(STL_Triangle, n));
    UTEST_COMPARE(STL_CoordsRawSize, offsetof(STL_Triangle, v1));
    UTEST_COMPARE(2*STL_CoordsRawSize, offsetof(STL_Triangle, v2));
    UTEST_COMPARE(3*STL_CoordsRawSize, offsetof(STL_Triangle, v3));
    UTEST_COMPARE(4*STL_CoordsRawSize, offsetof(STL_Triangle, attribute_byte_count));
    UTEST_ASSERT(sizeof(STL_Triangle) >= STL_TriangleBinaryRawSize);
    return nullptr;
}

static const char* TestSTL_triangleNormal()
{
    const unsigned udiff = 5;
    { // Doesn't fail on invalid facet
        const Vec3f n = STL_triangleNormal({});
        UTEST_ASSERT(float32_ulpEquals(n.x, 0.f, udiff));
        UTEST_ASSERT(float32_ulpEquals(n.y, 0.f, udiff));
        UTEST_ASSERT(float32_ulpEquals(n.z, 0.f, udiff));
    }
    return nullptr;
}

} // namespace gmio

GMIO_PRAGMA_MSVC_WARNING_POP()
