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

#include <gmio_support/stl_occ_brep.h>

#include "stl_occ_utils.h"

#include <BRep_Tool.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

gmio_stl_mesh gmio_stl_occmesh(const gmio_stl_occshape_iterator& it)
{
    gmio_stl_mesh mesh = {};
    mesh.cookie = &it;

    if (it.shape() != NULL) {
        TopLoc_Location loc;
        const TopoDS_Shape& sh = *it.shape();
        for (TopExp_Explorer expl(sh, TopAbs_FACE); expl.More(); expl.Next()) {
            const Handle_Poly_Triangulation& poly =
                    BRep_Tool::Triangulation(TopoDS::Face(expl.Current()), loc);
            if (!poly.IsNull())
                mesh.triangle_count += poly->NbTriangles();
        }
    }

    //mesh.func_get_triangle = internal::occshape_get_triangle;
    mesh.func_get_triangle = &gmio_stl_occshape_iterator::get_triangle;
    return mesh;
}


gmio_stl_occshape_iterator::gmio_stl_occshape_iterator()
    : m_shape(NULL)
{
    this->reset_face();
}

gmio_stl_occshape_iterator::gmio_stl_occshape_iterator(const TopoDS_Shape& shape)
    : m_shape(&shape),
      m_expl(shape, TopAbs_FACE)
{
    if (m_expl.More()) {
        this->cache_face(TopoDS::Face(m_expl.Current()));
    }
    else {
        this->reset_face();
    }
}

void gmio_stl_occshape_iterator::get_triangle(
        const void *cookie, uint32_t /*tri_id*/, gmio_stl_triangle *tri)
{
    void* wcookie = const_cast<void*>(cookie);
    gmio_stl_occshape_iterator* it =
            static_cast<gmio_stl_occshape_iterator*>(wcookie);

    const bool reversed = it->m_face_is_reversed;
    const gp_Trsf& trsf = it->m_face_trsf;
    const TColgp_Array1OfPnt* nodes = it->m_face_nodes;
    int n1, n2, n3; // Node index
    const Poly_Triangle& curr_tri =
            it->m_face_triangles->Value(it->m_face_tri_id);
    curr_tri.Get(n1, n2, n3);
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
    it->move_to_next_tri();
}

bool gmio_stl_occshape_iterator::move_to_next_tri()
{
    ++m_face_tri_id;
    if (m_face_tri_id > m_face_last_tri_id) {
        m_expl.Next();
        if (m_expl.More()) {
            this->cache_face(TopoDS::Face(m_expl.Current()));
            return true;
        }
        return false;
    }
    return true;
}

void gmio_stl_occshape_iterator::reset_face()
{
    m_face_poly = NULL;
    m_face_nodes = NULL;
    m_face_triangles = NULL;
    if (m_face_trsf.Form() != gp_Identity)
        m_face_trsf = gp_Trsf();
    m_face_is_reversed = false;
    m_face_tri_id = 0;
    m_face_last_tri_id = 0;
}

void gmio_stl_occshape_iterator::cache_face(const TopoDS_Face& face)
{
    TopLoc_Location loc;
    const Handle_Poly_Triangulation& hnd_face_poly =
            BRep_Tool::Triangulation(face, loc);
    m_face_trsf = loc.Transformation();
    m_face_poly =
            !hnd_face_poly.IsNull() ? hnd_face_poly.operator->() : NULL;
    m_face_nodes =
            m_face_poly != NULL ? &m_face_poly->Nodes() : NULL;
    m_face_triangles =
            m_face_poly != NULL ? &m_face_poly->Triangles() : NULL;
    m_face_is_reversed = face.Orientation() == TopAbs_REVERSED;
    if (m_face_trsf.IsNegative())
        m_face_is_reversed = !m_face_is_reversed;
    m_face_tri_id =
            m_face_triangles != NULL ? m_face_triangles->Lower() : -1;
    m_face_last_tri_id =
            m_face_triangles != NULL ? m_face_triangles->Upper() : -1;
}
