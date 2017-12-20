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

#include "stl_utils.h"

#include "../src/gmio_core/internal/numeric_utils.h"
#include "../src/gmio_core/internal/vecgeom_utils.h"

#include <algorithm>

namespace gmio {

bool STL_triangleEquals(
        const STL_Triangle& lhs, const STL_Triangle& rhs, uint32_t max_ulp_diff)
{
    return vec3_equals(lhs.n, rhs.n, max_ulp_diff)
            && vec3_equals(lhs.v1, rhs.v1, max_ulp_diff)
            && vec3_equals(lhs.v2, rhs.v2, max_ulp_diff)
            && vec3_equals(lhs.v3, rhs.v3, max_ulp_diff)
            && lhs.attribute_byte_count == rhs.attribute_byte_count;
}

void STL_MeshCreatorBasic::beginSolid(const STL_MeshCreatorInfos& infos)
{
    m_infos = infos;
    m_vec_triangle.resize(0);
    if (infos.format == STL_Format_Ascii) {
        // Try to guess how many vertices we could have assume we'll need
        // 200 bytes for each face
        const size_t facet_size = 200;
        const uint64_t facet_count = std::max(1ui64, infos.ascii_solid_size / facet_size);
        m_vec_triangle.reserve(facet_count);
    }
    else if ((infos.format & STL_Format_TagBinary) != 0) {
        m_vec_triangle.reserve(infos.binary_triangle_count);
    }
}

void STL_MeshCreatorBasic::addTriangle(uint32_t, const STL_Triangle &triangle)
{
    m_vec_triangle.push_back(triangle);
}

const STL_MeshCreatorInfos &STL_MeshCreatorBasic::infos() const
{
    return m_infos;
}

const std::vector<STL_Triangle> &STL_MeshCreatorBasic::triangles() const
{
    return m_vec_triangle;
}

STL_MeshBasic::STL_MeshBasic(const std::vector<STL_Triangle> &vec_triangle)
    : m_vec_triangle(vec_triangle)
{
    this->setTriangleCount(m_vec_triangle.size());
}

STL_Triangle STL_MeshBasic::triangle(uint32_t tri_id) const
{
    return m_vec_triangle.at(tri_id);
}

} // namespace gmio
