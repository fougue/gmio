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

/*! \file stl_occ_mesh.h
 *  STL support of OpenCascade's StlMesh_Mesh
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

#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshTriangle.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>

struct gmio_stl_occmesh_iterator;

/*! Returns a gmio_stl_mesh mapped to the OpenCascade mesh in iterator \p it
 *
 *  The mesh's cookie will point to \c &it so the lifescope of the corresponding
 *  object must be at least as longer as the returned gmio_stl_mesh.
 *
 *  Example of use:
 *  \code{.cpp}
 *      Handle_StlMesh_Mesh occmesh = ...;
 *      const gmio_stl_occmesh_iterator it(occmesh);
 *      const gmio_stl_mesh mesh = gmio_stl_occmesh(it);
 *      gmio_stl_write_file(stl_format, filepath, &mesh, &options);
 *  \endcode
 */
gmio_stl_mesh gmio_stl_occmesh(const gmio_stl_occmesh_iterator& it);

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


/*! Forward iterator over the triangles of OpenCascade's StlMesh_Mesh
 *
 *  It is used internally to iterate over the triangles of all domains within
 *  a StlMesh_Mesh object.
 */
struct gmio_stl_occmesh_iterator
{
    gmio_stl_occmesh_iterator();
    explicit gmio_stl_occmesh_iterator(const StlMesh_Mesh* mesh);
    explicit gmio_stl_occmesh_iterator(const Handle_StlMesh_Mesh& hnd);

    inline const StlMesh_Mesh* mesh() const { return m_mesh; }

private:
    friend gmio_stl_mesh gmio_stl_occmesh(const gmio_stl_occmesh_iterator&);
    static void get_triangle(
            const void* cookie, uint32_t tri_id, gmio_stl_triangle* tri);

    bool move_to_next_tri(uint32_t tri_id);
    void init(const StlMesh_Mesh* mesh);
    void cache_domain(int dom_id);

    const StlMesh_Mesh* m_mesh;
    int m_domain_count;
    int m_domain_id;
    const StlMesh_SequenceOfMeshTriangle* m_domain_triangles;
    const TColgp_SequenceOfXYZ* m_domain_vertices;
    uint32_t m_domain_first_tri_id;
    uint32_t m_domain_last_tri_id;
};

#endif /* GMIO_SUPPORT_STL_OCC_MESH_H */
/*! @} */
