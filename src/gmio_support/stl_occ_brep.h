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
 *  object must be at least as longer as the returned gmio_stl_mesh.
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
 *  It is used to iterate efficiently over the triangles of all internally
 *  triangulated sub faces
 *
 *  Don't use API of this class, it's intended to gmio_stl_occmesh()
 */
struct gmio_stl_occshape_iterator
{
    gmio_stl_occshape_iterator();
    explicit gmio_stl_occshape_iterator(const TopoDS_Shape& shape);

    inline const TopoDS_Shape* shape() const;

    bool move_to_next_tri();
    inline bool face_is_reversed() const;
    inline const gp_Trsf& face_trsf() const;
    inline const TColgp_Array1OfPnt* face_nodes() const;
    inline const Poly_Triangle* face_current_triangle() const;

private:
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


#ifndef DOXYGEN

/*
 * Implementation
 */

const TopoDS_Shape *gmio_stl_occshape_iterator::shape() const
{ return m_shape; }

bool gmio_stl_occshape_iterator::face_is_reversed() const
{ return m_face_is_reversed; }

const gp_Trsf &gmio_stl_occshape_iterator::face_trsf() const
{ return m_face_trsf; }

const TColgp_Array1OfPnt *gmio_stl_occshape_iterator::face_nodes() const
{ return m_face_nodes; }

const Poly_Triangle *gmio_stl_occshape_iterator::face_current_triangle() const
{ return &m_face_triangles->Value(m_face_tri_id); }

#endif /* !DOXYGEN */

#endif /* GMIO_SUPPORT_STL_OCC_BREP_H */
/*! @} */
