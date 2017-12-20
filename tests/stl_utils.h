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

#pragma once

#include "../src/gmio_core/global.h"
#include "../src/gmio_stl/stl_mesh.h"
#include "../src/gmio_stl/stl_mesh_creator.h"
#include "../src/gmio_stl/stl_triangle.h"

#include <vector>

namespace gmio {

bool STL_triangleEquals(
        const STL_Triangle& lhs, const STL_Triangle& rhs, uint32_t max_ulp_diff);

class STL_MeshBasic : public STL_Mesh {
public:
    STL_MeshBasic(const std::vector<STL_Triangle>& vec_triangle);
    STL_Triangle triangle(uint32_t tri_id) const override;

private:
    const std::vector<STL_Triangle>& m_vec_triangle;
};

class STL_MeshCreatorBasic : public STL_MeshCreator {
public:
    void beginSolid(const STL_MeshCreatorInfos& infos) override;
    void addTriangle(uint32_t tri_id, const STL_Triangle& triangle) override;

    const STL_MeshCreatorInfos& infos() const;
    const std::vector<STL_Triangle>& triangles() const;

private:
    STL_MeshCreatorInfos m_infos;
    std::vector<STL_Triangle> m_vec_triangle;
};

} // namespace gmio
