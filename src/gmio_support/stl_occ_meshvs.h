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

#include <MeshVS_DataSource.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

struct gmio_stl_occmesh_datasource_iterator;

/*! Returns a gmio_stl_mesh mapped to the OpenCascade mesh data-source in
 *  iterator \p it
 *
 *  The mesh's cookie will point to \c &it so the lifescope of the corresponding
 *  object must be at least as longer as the returned gmio_stl_mesh.
 *
 *  Example of use:
 *  \code{.cpp}
 *      Handle_MeshVS_Mesh mesh = ...;
 *      const gmio_stl_occmesh_datasource_iterator it(mesh->GetDataSource());
 *      const gmio_stl_mesh mesh = gmio_stl_occmesh(it);
 *      gmio_stl_write_file(stl_format, filepath, &mesh, &options);
 *  \endcode
 */
gmio_stl_mesh gmio_stl_occmesh(const gmio_stl_occmesh_datasource_iterator& it);

/*! Forward iterator over the triangles of OpenCascade's MeshVS_DataSource
 *
 *  It is used to iterate efficiently over the elements of a MeshVS_DataSource
 *  object.\n
 *  Each element should be of type MeshVS_ET_Face and made of 3 nodes.
 *
 *  Don't use API of this class, it's intended to gmio_stl_occmesh()
 */
struct gmio_stl_occmesh_datasource_iterator
{
    gmio_stl_occmesh_datasource_iterator();
    explicit gmio_stl_occmesh_datasource_iterator(const MeshVS_DataSource* data_src);
    explicit gmio_stl_occmesh_datasource_iterator(const Handle_MeshVS_DataSource& hnd);

    inline void move_to_next_tri();
    inline int current_element_key() const;
    inline TColStd_Array1OfReal& cached_element_coords();
    inline const MeshVS_DataSource* data_src() const;

private:
    const MeshVS_DataSource* m_data_src;
    TColStd_MapIteratorOfPackedMapOfInteger m_element_it;
    TColStd_Array1OfReal m_element_coords;
};

#ifndef DOXYGEN

/*
 * Implementation
 */

void gmio_stl_occmesh_datasource_iterator::move_to_next_tri()
{ m_element_it.Next(); }

int gmio_stl_occmesh_datasource_iterator::current_element_key() const
{ return m_element_it.Key(); }

TColStd_Array1OfReal& gmio_stl_occmesh_datasource_iterator::cached_element_coords()
{ return m_element_coords; }

const MeshVS_DataSource* gmio_stl_occmesh_datasource_iterator::data_src() const
{ return m_data_src; }

#endif /* !DOXYGEN */

#endif /* GMIO_SUPPORT_STL_OCC_MESHVS_H */
/*! @} */
