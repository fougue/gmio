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

#include <vector>

#include <Poly_Triangulation.hxx>
#include <TopoDS_Shape.hxx>
class TopoDS_Face;

/*! Provides access to all the internal triangles of OpenCascade's
 *  \c TopoDS_Shape
 *
 *  gmio_stl_mesh_occshape iterates efficiently over the triangles of all
 *  sub <tt>TopoDS_Faces</tt>(internal \c Poly_Triangulation objects).
 *
 *  Example of use:
 *  \code{.cpp}
 *      const TopoDS_Shape occshape = ...;
 *      const gmio_stl_mesh_occshape mesh(occshape);
 *      gmio_stl_write_file(stl_format, filepath, &occmesh, &options);
 *  \endcode
 */
struct gmio_stl_mesh_occshape : public gmio_stl_mesh
{
    gmio_stl_mesh_occshape();
    explicit gmio_stl_mesh_occshape(const TopoDS_Shape& shape);

    inline const TopoDS_Shape* shape() const { return m_shape; }

private:
    static void get_triangle(
            const void* cookie, uint32_t tri_id, gmio_stl_triangle* tri);

    void init_C_members();

    struct face_data {
        gp_Trsf trsf;
        bool is_reversed;
        const TColgp_Array1OfPnt* ptr_nodes;
    };
    struct triangle_data {
        const Poly_Triangle* ptr_triangle;
        const face_data* ptr_face_data;
    };

    std::vector<face_data> m_vec_face_data;
    std::vector<triangle_data> m_vec_triangle_data;
    const TopoDS_Shape* m_shape;
};

#endif /* GMIO_SUPPORT_STL_OCC_BREP_H */
/*! @} */
