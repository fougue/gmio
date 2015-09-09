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

/*! \file stl_occ.h
 *  Support of OpenCascade's StlMesh_Mesh
 *
*  \addtogroup gmio_support
 *  @{
 */

#ifndef __cplusplus
#  error C++ compiler required
#endif

#ifndef GMIO_SUPPORT_STL_OCC_H
#define GMIO_SUPPORT_STL_OCC_H

#include "support_global.h"
#include "../gmio_stl/stl_mesh.h"
#include "../gmio_stl/stl_mesh_creator.h"

class Handle_StlMesh_Mesh;
class StlMesh_Mesh;

/*! Domain in a OpenCascade \c StlMesh_Mesh object
 *
 *  The domain is indicated with its index within the STL mesh
 */
struct GMIO_LIBSUPPORT_EXPORT gmio_occ_stl_mesh_domain
{
    gmio_occ_stl_mesh_domain();
    gmio_occ_stl_mesh_domain(const StlMesh_Mesh* mesh, int dom_id = 1);
    gmio_occ_stl_mesh_domain(const Handle_StlMesh_Mesh& hndMesh, int dom_id = 1);
    const StlMesh_Mesh* mesh;
    int domain_id;
};
typedef struct gmio_occ_stl_mesh_domain gmio_occ_stl_mesh_domain_t;

/*! Returns a gmio_stl_mesh mapped to domain in StlMesh_Mesh
 *
 *  The mesh's cookie will point to \p mesh_domain
 */
GMIO_LIBSUPPORT_EXPORT
gmio_stl_mesh_t gmio_stl_occmesh(const gmio_occ_stl_mesh_domain_t* mesh_domain);

/*! Returns a gmio_stl_mesh_creator that will build a new domain in a
 *  StlMesh_Mesh object
 *
 *  The creator's cookie will point \p mesh
 */
GMIO_LIBSUPPORT_EXPORT
gmio_stl_mesh_creator_t gmio_stl_occmesh_creator(StlMesh_Mesh* mesh);

/*! Same as gmio_stl_occmesh_creator(StlMesh_Mesh*) but takes a handle
 *
 *  The creator's cookie will point to the internal data(ie StlMesh_Mesh*) of
 *  handle \p hnd
 */
GMIO_LIBSUPPORT_EXPORT
gmio_stl_mesh_creator_t gmio_stl_hnd_occmesh_creator(const Handle_StlMesh_Mesh& hnd);

#endif /* GMIO_SUPPORT_STL_OCC_H */
/*! @} */
