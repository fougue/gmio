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

#include <gmio_support/stl_occ_brep.h>

#include "stl_occ_utils.h"

#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

gmio_stl_mesh_occshape::gmio_stl_mesh_occshape()
    : m_shape(NULL)
{
    this->init_C_members();
}

gmio_stl_mesh_occshape::gmio_stl_mesh_occshape(const TopoDS_Shape& shape)
    : m_shape(&shape)
{
    this->init_C_members();

    // Count facets and triangles
    std::size_t face_count = 0;
    for (TopExp_Explorer expl(shape, TopAbs_FACE); expl.More(); expl.Next()) {
        TopLoc_Location loc;
        const Handle_Poly_Triangulation& hnd_face_poly =
                BRep_Tool::Triangulation(TopoDS::Face(expl.Current()), loc);
        if (!hnd_face_poly.IsNull()) {
            ++face_count;
            this->triangle_count += hnd_face_poly->NbTriangles();
        }
    }

    // Fill face and triangle datas
    m_vec_face_data.reserve(face_count);
    m_vec_triangle_data.reserve(this->triangle_count);
    for (TopExp_Explorer expl(shape, TopAbs_FACE); expl.More(); expl.Next()) {
        const TopoDS_Face& topoface = TopoDS::Face(expl.Current());
        TopLoc_Location loc;
        const Handle_Poly_Triangulation& hnd_face_poly =
                BRep_Tool::Triangulation(topoface, loc);
        if (!hnd_face_poly.IsNull()) {
            {   // Add next face_data
                struct face_data facedata;
                facedata.trsf = loc.Transformation();
                facedata.is_reversed = (topoface.Orientation() == TopAbs_REVERSED);
                if (facedata.trsf.IsNegative())
                    facedata.is_reversed = !facedata.is_reversed;
                facedata.ptr_nodes = &hnd_face_poly->Nodes();
                m_vec_face_data.push_back(std::move(facedata));
            }
            const struct face_data& last_facedata = m_vec_face_data.back();
            // Add triangle_datas
            const Poly_Array1OfTriangle& vec_face_tri = hnd_face_poly->Triangles();
            for (int i = vec_face_tri.Lower(); i <= vec_face_tri.Upper(); ++i) {
                struct triangle_data tridata;
                tridata.ptr_triangle = &vec_face_tri.Value(i);
                tridata.ptr_face_data = &last_facedata;
                m_vec_triangle_data.push_back(std::move(tridata));
            }
        }
    }
}

// static
void gmio_stl_mesh_occshape::get_triangle(
        const void *cookie, uint32_t tri_id, gmio_stl_triangle *tri)
{
    const gmio_stl_mesh_occshape* it =
            static_cast<const gmio_stl_mesh_occshape*>(cookie);

    const struct triangle_data* tridata = &it->m_vec_triangle_data.at(tri_id);
    const struct face_data* facedata = tridata->ptr_face_data;
    const bool reversed = facedata->is_reversed;
    const gp_Trsf& trsf = facedata->trsf;
    const TColgp_Array1OfPnt* nodes = facedata->ptr_nodes;
    int n1, n2, n3; // Node index
    const Poly_Triangle* occtri = tridata->ptr_triangle;
    occtri->Get(n1, n2, n3);
    gp_Pnt p1 = nodes->Value(n1);
    gp_Pnt p2 = nodes->Value(reversed ? n3 : n2);
    gp_Pnt p3 = nodes->Value(reversed ? n2 : n3);
    if (trsf.Form() != gp_Identity) {
        p1.Transform(trsf);
        p2.Transform(trsf);
        p3.Transform(trsf);
    }
    gmio_stl_occ_copy_xyz(&tri->v1, p1.XYZ());
    gmio_stl_occ_copy_xyz(&tri->v2, p2.XYZ());
    gmio_stl_occ_copy_xyz(&tri->v3, p3.XYZ());
    gmio_stl_triangle_compute_normal(tri);
}

void gmio_stl_mesh_occshape::init_C_members()
{
    this->cookie = this;
    this->func_get_triangle = &gmio_stl_mesh_occshape::get_triangle;
    this->triangle_count = 0;
}
