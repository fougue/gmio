/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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

/*! \file stl_occ_mesh.h
 *  STL support of OpenCascade's StlMesh_Mesh
 *
 *  To use this header the source file
 *      <tt>$INSTALL/src/gmio_support/stl_occ_mesh.cpp</tt>\n
 *  needs to be built in the target project(\c $INSTALL is the root directory
 *  where is installed gmio)
 *
 *  Of course this requires the includepath and libpath to point to OpenCascade,
 *  the import libraries likely needed are:\n
 *      <tt>TKernel TKMath TKSTL TKTopAlgo</tt>
 *
 *  \addtogroup gmio_support
 *  @{
 */

#ifndef __cplusplus
#  error C++ compiler required
#endif

#ifndef GMIO_SUPPORT_STL_OCC_MESH_H
#define GMIO_SUPPORT_STL_OCC_MESH_H

#include "support_global.h"
#include "../gmio_stl/stl_mesh.h"
#include "../gmio_stl/stl_mesh_creator.h"

#include <vector>

#include <BRepBuilderAPI_CellFilter.hxx>
#include <BRepBuilderAPI_VertexInspector.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>

/*! Provides access to all the triangles of OpenCascade's \c StlMesh_Mesh
 *
 *  gmio_stl_mesh_occmesh iterates efficiently over the triangles of all
 *  domains.
  *
 *  Example of use:
 *  \code{.cpp}
 *      const Handle_StlMesh_Mesh occmesh = ...;
 *      const gmio_stl_mesh_occmesh mesh(occmesh);
 *      gmio_stl_write_file(stl_format, filepath, &mesh, &options);
 *  \endcode
 */
struct gmio_stl_mesh_occmesh : public gmio_stl_mesh
{
    gmio_stl_mesh_occmesh();
    explicit gmio_stl_mesh_occmesh(const StlMesh_Mesh* mesh);
    explicit gmio_stl_mesh_occmesh(const Handle_StlMesh_Mesh& hnd);

    inline const StlMesh_Mesh* mesh() const { return m_mesh; }

private:
    static void get_triangle(
            const void* cookie, uint32_t tri_id, gmio_stl_triangle* tri);

    void init_C_members();
    void init_cache();

    struct domain_data {
        std::vector<const gp_XYZ*> vec_coords;
    };

    struct triangle_data {
        const StlMesh_MeshTriangle* ptr_triangle;
        const domain_data* ptr_domain;
    };

    const StlMesh_Mesh* m_mesh;
    std::vector<domain_data> m_vec_domain_data;
    std::vector<triangle_data> m_vec_triangle_data;
};

/*! Provides creation of a new domain within an StlMesh_Mesh object
 *
 *  gmio_stl_mesh_creator::func_add_triangle() calls
 *  <tt>StlMesh_Mesh::AddVertex()</tt> only for new unique vertices, ie. they
 *  are no vertex duplicates in the resulting domain.
 *
 *  As of OpenCascade v7.0.0, it's not possible to rely on
 *  <tt>StlMesh_Mesh::AddOnlyNewVertex()</tt>: this function
 *  still has the same effect as <tt>StlMesh_Mesh::AddVertex()</tt>
 *
 *  Example of use:
 *  \code{.cpp}
 *      Handle_StlMesh_Mesh occmesh = new StlMesh_Mesh;
 *      gmio_stl_mesh_creator_occmesh meshcreator(occmesh);
 *      gmio_stl_read_file(filepath, &meshcreator, &options);
 *  \endcode
 */
struct gmio_stl_mesh_creator_occmesh : public gmio_stl_mesh_creator
{
    gmio_stl_mesh_creator_occmesh();
    explicit gmio_stl_mesh_creator_occmesh(StlMesh_Mesh* mesh);
    explicit gmio_stl_mesh_creator_occmesh(const Handle_StlMesh_Mesh& hnd);

    inline StlMesh_Mesh* mesh() const { return m_mesh; }

private:
    static void begin_solid(
            void* cookie, const struct gmio_stl_mesh_creator_infos* infos);
    static void add_triangle(
            void* cookie, uint32_t tri_id, const gmio_stl_triangle* tri);

    void init_C_members();
    int add_unique_vertex(const gmio_vec3f& v);

    StlMesh_Mesh* m_mesh;
    BRepBuilderAPI_CellFilter m_filter;
    BRepBuilderAPI_VertexInspector m_inspector;
};

#endif /* GMIO_SUPPORT_STL_OCC_MESH_H */
/*! @} */
