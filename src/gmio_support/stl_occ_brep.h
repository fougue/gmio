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

/*! \file stl_occ_brep.h
 *  STL support of OpenCascade's TopoDS_Shape
 *
 *  To use this header the source file
 *      <tt>$INSTALL/src/gmio_support/stl_occ_brep.cpp</tt>\n
 *  needs to be built in the target project(\c $INSTALL is the root directory
 *  where is installed gmio)
 *
 *  Of course this requires the includepath and libpath to point to OpenCascade,
 *  the import libraries likely needed are:\n
 *      <tt>TKernel TKMath TKBRep</tt>
 *
 *  \addtogroup gmio_support
 *  @{
 */

#ifndef __cplusplus
#  error C++ compiler required
#endif

#ifndef GMIO_SUPPORT_STL_OCC_BREP_H
#define GMIO_SUPPORT_STL_OCC_BREP_H

#include "support_global.h"
#include "../gmio_stl/stl_mesh.h"

#include <Poly_Triangulation.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
class TopoDS_Face;

struct gmio_stl_occshape_iterator;

/*! Returns a gmio_stl_mesh mapped to the OpenCascade mesh in iterator \p it
 *
 *  The mesh's cookie will point to \c &it so the lifescope of the corresponding
 *  object must be at least as long as the returned gmio_stl_mesh.
 *
 *  Example of use:
 *  \code{.cpp}
 *      const TopoDS_Shape occshape = ...;
 *      const gmio_stl_occshape_iterator it(occshape);
 *      const gmio_stl_mesh mesh = gmio_stl_occmesh(it);
 *      gmio_stl_write_file(stl_format, filepath, &mesh, &options);
 *  \endcode
 */
gmio_stl_mesh gmio_stl_occmesh(const gmio_stl_occshape_iterator& it);


/*! Forward iterator over the triangles of OpenCascade's TopoDS_Shape
 *
 *  It is used to iterate over the triangles of all triangulated sub faces(the
 *  Poly_Triangulation object).
 */
struct gmio_stl_occshape_iterator
{
    gmio_stl_occshape_iterator();
    explicit gmio_stl_occshape_iterator(const TopoDS_Shape& shape);

    inline const TopoDS_Shape* shape() const { return m_shape; }

private:
    friend gmio_stl_mesh gmio_stl_occmesh(const gmio_stl_occshape_iterator&);
    static void get_triangle(
            const void* cookie, uint32_t tri_id, gmio_stl_triangle* tri);

    bool move_to_next_tri();
    void reset_face();
    void cache_face(const TopoDS_Face& face);

    const TopoDS_Shape* m_shape;
    TopExp_Explorer m_expl;
    const Poly_Triangulation* m_face_poly;
    const TColgp_Array1OfPnt* m_face_nodes;
    const Poly_Array1OfTriangle* m_face_triangles;
    gp_Trsf m_face_trsf;
    bool m_face_is_reversed;
    int m_face_tri_id;
    int m_face_last_tri_id;
};

#endif /* GMIO_SUPPORT_STL_OCC_BREP_H */
/*! @} */
