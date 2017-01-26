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
