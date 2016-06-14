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

/*! \file stl_occ_meshvs.h
 *  STL support of OpenCascade's MeshVS_DataSource
 *
 *  To use this header the source file
 *      <tt>$INSTALL/src/gmio_support/stl_occ_meshvs.cpp</tt>\n
 *  needs to be built in the target project(\c $INSTALL is the root directory
 *  where is installed gmio)
 *
 *  Of course this requires the includepath and libpath to point to OpenCascade,
 *  the import libraries likely needed are:\n
 *      <tt>TKernel TKMath TKMeshVS</tt>
 *
 *  \addtogroup gmio_support
 *  @{
 */

#ifndef __cplusplus
#  error C++ compiler required
#endif

#ifndef GMIO_SUPPORT_STL_OCC_MESHVS_H
#define GMIO_SUPPORT_STL_OCC_MESHVS_H

#include "support_global.h"
#include "../gmio_stl/stl_mesh.h"

#include <vector>

#include <MeshVS_DataSource.hxx>
#include <TColStd_Array1OfReal.hxx>

/*! Provides access to all the triangles of OpenCascade's \c MeshVS_DataSource
 *
 *  gmio_stl_mesh_occmeshvs iterates efficiently over the elements of a
 *  \c MeshVS_DataSource object.\n
 *  Each element should be of type \c MeshVS_ET_Face and made of 3 nodes.
 *
 *  Example of use:
 *  \code{.cpp}
 *      Handle_MeshVS_Mesh occmeshvs = ...;
 *      const gmio_stl_mesh_occmeshvs mesh(occmeshvs);
 *      gmio_stl_write_file(stl_format, filepath, &mesh, &options);
 *  \endcode
 */
struct gmio_stl_mesh_occmeshvs : public gmio_stl_mesh
{
    gmio_stl_mesh_occmeshvs();
    explicit gmio_stl_mesh_occmeshvs(const MeshVS_DataSource* ds);
    explicit gmio_stl_mesh_occmeshvs(const Handle_MeshVS_DataSource& hnd);

    inline const MeshVS_DataSource* data_src() const { return m_data_src; }

private:
    static void get_triangle(
            const void* cookie, uint32_t tri_id, gmio_stl_triangle* tri);

    void init_C_members();
    void init_cache();

    const MeshVS_DataSource* m_data_src;
    std::vector<int> m_vec_element_key;
    mutable TColStd_Array1OfReal m_element_coords;
};

#endif /* GMIO_SUPPORT_STL_OCC_MESHVS_H */
/*! @} */
