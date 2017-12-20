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

#include <gmio_support/stl_occ_brep.h>
#include "stl_occ_utils.h"

#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

namespace gmio {

STL_MeshOccShape::STL_MeshOccShape(const TopoDS_Shape& shape)
    : m_shape(shape)
{
    // Count facets and triangles
    size_t face_count = 0;
    uint32_t tri_count = 0;
    for (TopExp_Explorer expl(shape, TopAbs_FACE); expl.More(); expl.Next()) {
        TopLoc_Location loc;
        const Handle_Poly_Triangulation& hnd_face_poly =
                BRep_Tool::Triangulation(TopoDS::Face(expl.Current()), loc);
        if (!hnd_face_poly.IsNull()) {
            ++face_count;
            tri_count += hnd_face_poly->NbTriangles();
        }
    }
    this->setTriangleCount(tri_count);

    // Fill face and triangle datas
    size_t vec_face_id = 0;
    size_t vec_tri_id = 0;
    m_vec_face_data.resize(face_count);
    m_vec_triangle_data.resize(tri_count);
    for (TopExp_Explorer expl(shape, TopAbs_FACE); expl.More(); expl.Next()) {
        const TopoDS_Face& topoface = TopoDS::Face(expl.Current());
        TopLoc_Location loc;
        const Handle_Poly_Triangulation& hnd_face_poly =
                BRep_Tool::Triangulation(topoface, loc);
        if (!hnd_face_poly.IsNull()) {
            // Copy next face_data
            FaceData& facedata = m_vec_face_data.at(vec_face_id);
            facedata.trsf = loc.Transformation();
            facedata.is_reversed = (topoface.Orientation() == TopAbs_REVERSED);
            if (facedata.trsf.IsNegative())
                facedata.is_reversed = !facedata.is_reversed;
            facedata.ptr_nodes = &hnd_face_poly->Nodes();

            // Copy triangle_datas
            const Poly_Array1OfTriangle& vec_face_tri = hnd_face_poly->Triangles();
            for (int i = vec_face_tri.Lower(); i <= vec_face_tri.Upper(); ++i) {
                TriangleData& tridata = m_vec_triangle_data.at(vec_tri_id);
                tridata.ptr_triangle = &vec_face_tri.Value(i);
                tridata.ptr_face_data = &facedata;
                ++vec_tri_id;
            }

            ++vec_face_id;
        }
    }
}

STL_Triangle STL_MeshOccShape::triangle(uint32_t tri_id) const
{
    const TriangleData& tridata = m_vec_triangle_data.at(tri_id);
    const FaceData* facedata = tridata.ptr_face_data;
    const bool reversed = facedata->is_reversed;
    const gp_Trsf& trsf = facedata->trsf;
    const TColgp_Array1OfPnt* nodes = facedata->ptr_nodes;
    int n1, n2, n3; // Node index
    const Poly_Triangle* occtri = tridata.ptr_triangle;
    occtri->Get(n1, n2, n3);
    gp_Pnt p1 = nodes->Value(n1);
    gp_Pnt p2 = nodes->Value(reversed ? n3 : n2);
    gp_Pnt p3 = nodes->Value(reversed ? n2 : n3);
    if (trsf.Form() != gp_Identity) {
        p1.Transform(trsf);
        p2.Transform(trsf);
        p3.Transform(trsf);
    }

    STL_Triangle tri;
    tri.attribute_byte_count = 0;
    OCC_copyCoords(&tri.v1, p1.XYZ());
    OCC_copyCoords(&tri.v2, p2.XYZ());
    OCC_copyCoords(&tri.v3, p3.XYZ());
    tri.n = STL_triangleNormal(tri);
    return tri;
}

// -----------------------------------------------------------------------------
// STL_MeshCreatorOccShape
// -----------------------------------------------------------------------------

STL_MeshCreatorOccShape::STL_MeshCreatorOccShape()
{
}

void STL_MeshCreatorOccShape::endSolid()
{
    STL_MeshCreatorOccPolyTriangulation::endSolid();
    if (!this->polytri().IsNull()) {
        Handle_BRep_TFace face = new BRep_TFace;
        face->Triangulation(this->polytri());
        m_shape.TShape(face);
    }
}

} //namespace gmio
