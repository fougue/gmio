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

#include <gmio_support/stl_occ_meshvs.h>

#include "stl_occ_utils.h"

#include <MeshVS_DataSource.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

namespace internal {

static void occmesh_datasource_get_triangle(
        const void* cookie, uint32_t /*tri_id*/, gmio_stl_triangle* tri)
{
    void* wcookie = const_cast<void*>(cookie);
    gmio_stl_occmesh_datasource_iterator* it =
            static_cast<gmio_stl_occmesh_datasource_iterator*>(wcookie);
    const MeshVS_DataSource* data_src = it->data_src();

    int node_count;
    MeshVS_EntityType entity_type;
    const Standard_Boolean get_geom_ok =
            data_src->GetGeom(
                it->current_element_key(),
                Standard_True, // Is element
                it->cached_element_coords(),
                node_count,
                entity_type);
    if (get_geom_ok && node_count == 3) {
        // Copy vertex coords
        const TColStd_Array1OfReal& in_coords_array = it->cached_element_coords();
        float* out_coords_ptr = &tri->v1.x;
        for (int i = 0; i < 9; ++i)
            out_coords_ptr[i] = static_cast<float>(in_coords_array.Value(i + 1));
        // Copy normal coords
        double nx, ny, nz;
        data_src->GetNormal(it->current_element_key(), 3, nx, ny, nz);
        gmio_stl_occ_copy_xyz(&tri->n, nx, ny, nz);
    }
    it->move_to_next_tri();
}

} // namespace internal

gmio_stl_mesh gmio_stl_occmesh(const gmio_stl_occmesh_datasource_iterator& it)
{
    gmio_stl_mesh mesh = {};
    mesh.cookie = &it;
    mesh.triangle_count =
            it.data_src() != NULL ?
                it.data_src()->GetAllElements().Extent() : 0;
    mesh.func_get_triangle = internal::occmesh_datasource_get_triangle;
    return mesh;
}

gmio_stl_occmesh_datasource_iterator::gmio_stl_occmesh_datasource_iterator()
    : m_data_src(NULL),
      m_element_coords(1, 1)
{ }

gmio_stl_occmesh_datasource_iterator::gmio_stl_occmesh_datasource_iterator(
        const MeshVS_DataSource *data_src)
    : m_data_src(data_src),
      m_element_coords(1, 9)
{
    if (m_data_src != NULL)
        m_element_it.Initialize(m_data_src->GetAllElements());
}

gmio_stl_occmesh_datasource_iterator::gmio_stl_occmesh_datasource_iterator(
        const Handle_MeshVS_DataSource &hnd)
    : m_data_src(hnd.operator->()),
      m_element_coords(1, 9)
{
    if (m_data_src != NULL)
        m_element_it.Initialize(m_data_src->GetAllElements());
}
