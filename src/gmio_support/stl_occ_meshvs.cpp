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

#include <gmio_support/stl_occ_meshvs.h>

#include "stl_occ_utils.h"

#include <MeshVS_DataSource.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

#include <cstddef>

gmio_stl_mesh_occmeshvs::gmio_stl_mesh_occmeshvs()
    : m_data_src(NULL),
      m_element_coords(1, 1)
{
    this->init_C_members();
}

gmio_stl_mesh_occmeshvs::gmio_stl_mesh_occmeshvs(const MeshVS_DataSource *ds)
    : m_data_src(ds),
      m_element_coords(1, 9)
{
    this->init_C_members();
    this->init_cache();
}

gmio_stl_mesh_occmeshvs::gmio_stl_mesh_occmeshvs(const Handle_MeshVS_DataSource &hnd)
    : m_data_src(hnd.operator->()),
      m_element_coords(1, 9)
{
    this->init_C_members();
    this->init_cache();
}

void gmio_stl_mesh_occmeshvs::init_C_members()
{
    this->cookie = this;
    this->func_get_triangle = &gmio_stl_mesh_occmeshvs::get_triangle;
    this->triangle_count = 0;
}

void gmio_stl_mesh_occmeshvs::init_cache()
{
    if (m_data_src == NULL)
        return;

    this->triangle_count = m_data_src->GetAllElements().Extent();
    m_vec_element_key.reserve(this->triangle_count);

    TColStd_MapIteratorOfPackedMapOfInteger element_it;
    element_it.Initialize(m_data_src->GetAllElements());
    while (element_it.More()) {
        m_vec_element_key.push_back(element_it.Key());
        element_it.Next();
    }
}

void gmio_stl_mesh_occmeshvs::get_triangle(
        const void *cookie, uint32_t tri_id, gmio_stl_triangle *tri)
{
    const gmio_stl_mesh_occmeshvs* it =
            static_cast<const gmio_stl_mesh_occmeshvs*>(cookie);
    const MeshVS_DataSource* data_src = it->data_src();
    const int element_key = it->m_vec_element_key.at(tri_id);
    TColStd_Array1OfReal& element_coords = it->m_element_coords;

    int node_count;
    MeshVS_EntityType entity_type;
    const Standard_Boolean get_geom_ok =
            data_src->GetGeom(
                element_key,
                Standard_True, // Is element
                element_coords,
                node_count,
                entity_type);
    if (get_geom_ok && node_count == 3) {
        // Copy vertex coords
        const TColStd_Array1OfReal& in_coords_array = element_coords;
        float* out_coords_ptr = &tri->v1.x;
        for (int i = 0; i < 9; ++i)
            out_coords_ptr[i] = static_cast<float>(in_coords_array.Value(i + 1));
        // Copy normal coords
        double nx, ny, nz;
        data_src->GetNormal(element_key, 3, nx, ny, nz);
        gmio_stl_occ_copy_xyz(&tri->n, nx, ny, nz);
    }
}
