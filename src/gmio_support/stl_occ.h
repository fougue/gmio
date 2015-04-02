/****************************************************************************
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
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

/*! \file stl_occ.h
 *  Support of OpenCascade's StlMesh_Mesh
 */

#ifndef GMIO_SUPPORT_STL_OCC_H
#define GMIO_SUPPORT_STL_OCC_H

#include "support_global.h"
#include "../gmio_stl/stl_mesh.h"
#include "../gmio_stl/stl_mesh_creator.h"

#include <Handle_StlMesh_Mesh.hxx>
struct gmio_stl_mesh;
struct gmio_stl_mesh_creator;

/*! Domain in a OpenCascade \c StlMesh_Mesh object
 *
 *  The domain is indicated with its index within the STL mesh
 */
class GMIO_LIBSUPPORT_EXPORT gmio_OccStlMeshDomain
{
public:
    gmio_OccStlMeshDomain(const Handle_StlMesh_Mesh& stlMesh, int domId = 1);

    const Handle_StlMesh_Mesh& mesh() const;
    void setMesh(const Handle_StlMesh_Mesh& stlMesh);

    int domainId() const;
    void setDomainId(int domId);

private:
    Handle_StlMesh_Mesh m_mesh;
    int m_domainId;
};

/*! Initializes \p mesh so it maps to a domain in StlMesh_Mesh
 *
 *  \c mesh->cookie will point to \p meshCookie
 */
GMIO_LIBSUPPORT_EXPORT
void gmio_stl_set_occmesh(
        gmio_stl_mesh* mesh, const gmio_OccStlMeshDomain& meshCookie);

/*! Returns a gmio_stl_mesh mapped to domain in StlMesh_Mesh
 *
 *  The mesh's cookie will point to \p meshCookie
 */
GMIO_LIBSUPPORT_EXPORT
gmio_stl_mesh gmio_stl_occmesh(const gmio_OccStlMeshDomain& meshCookie);

/*! Initializes \p creator to build a new domain in a StlMesh_Mesh object
 *
 *  \c creator->cookie will point to the internal data(ie StlMesh_Mesh*) of
 *  handle \p mesh
 */
GMIO_LIBSUPPORT_EXPORT
void gmio_stl_set_occmesh_creator(
        gmio_stl_mesh_creator* creator, const Handle_StlMesh_Mesh& mesh);

/*! Returns a gmio_stl_mesh_creator that will build a new domain in a
 *  StlMesh_Mesh object
 *
 *  The creator's cookie will point to the internal data(ie StlMesh_Mesh*) of
 *  handle \p mesh
 */
GMIO_LIBSUPPORT_EXPORT
gmio_stl_mesh_creator gmio_stl_occmesh_creator(const Handle_StlMesh_Mesh& mesh);

#endif /* GMIO_SUPPORT_STL_OCC_H */
