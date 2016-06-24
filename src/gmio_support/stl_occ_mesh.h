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
 *      <tt>TKernel TKMath TKSTL</tt>
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

    struct triangle_data
    {
        const StlMesh_MeshTriangle* ptr_triangle;
        const TColgp_SequenceOfXYZ* ptr_vec_vertices;
    };

    const StlMesh_Mesh* m_mesh;
    int m_mesh_domain_count;

    // Data to be used when mesh domain_count > 1
    std::vector<triangle_data> m_vec_triangle_data;
    // Data to be used when mesh domain_count == 1
    const StlMesh_SequenceOfMeshTriangle* m_seq_triangle;
    const TColgp_SequenceOfXYZ* m_seq_vertex;
};

/*! Returns a gmio_stl_mesh_creator that will build a new domain in a
 *  StlMesh_Mesh object
 *
 *  The creator's cookie will point \p mesh
 */
gmio_stl_mesh_creator gmio_stl_occmesh_creator(StlMesh_Mesh* mesh);

/*! Same as gmio_stl_occmesh_creator(StlMesh_Mesh*) but takes a handle
 *
 *  The creator's cookie will point to the internal data(ie StlMesh_Mesh*) of
 *  handle \p hnd
 */
gmio_stl_mesh_creator gmio_stl_occmesh_creator(const Handle_StlMesh_Mesh& hnd);

#endif /* GMIO_SUPPORT_STL_OCC_MESH_H */
/*! @} */
