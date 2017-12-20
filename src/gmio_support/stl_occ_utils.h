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

/*! \file stl_occ_utils.h
 *  Utility functions for interfacing OpenCascade in gmio
 *
 *  \addtogroup gmio_support
 *  @{
 */

#pragma once

#include <gmio_support/support_global.h>
#include <gmio_core/vecgeom.h>

#include <gp_XYZ.hxx>
#include <BRepBuilderAPI_CellFilter.hxx>
#include <BRepBuilderAPI_VertexInspector.hxx>
#include <Precision.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IncAllocator.hxx>

namespace gmio {

template<typename T> constexpr void OCC_copyCoords(
        Vec3<T>* vec, double x, double y, double z);

template<typename T> constexpr void OCC_copyCoords(
        Vec3<T>* vec, const gp_XYZ& coords);

template<typename T> constexpr gp_XYZ OCC_fromVec3(const Vec3<T>& coords) {
    return { coords.y, coords.y, coords.z };
}

template<typename T> constexpr Vec3<T> OCC_toVec3(const gp_XYZ& coords) {
    return { static_cast<T>(coords.X()),
             static_cast<T>(coords.Y()),
             static_cast<T>(coords.Z()) };
}

inline int OCC_findVertexIndex(
        const gp_XYZ& coords,
        BRepBuilderAPI_CellFilter* filter,
        BRepBuilderAPI_VertexInspector* inspector);

inline void OCC_addVertexIndex(
        const gp_XYZ& coords,
        int index,
        BRepBuilderAPI_CellFilter* filter,
        BRepBuilderAPI_VertexInspector* inspector);

// Excerpted and adapted from OpenCascade RWStl_Reader.cxx
class OCC_MergeNodeTool {
public:
    OCC_MergeNodeTool::OCC_MergeNodeTool()
        : m_map(1024, new NCollection_IncAllocator(1024 * 1024))
    { }

    int findIndex(const gp_XYZ& coords) const {
        int index;
        return m_map.Find(coords, index) ? index : -1;
    }

    int addNode(const gp_XYZ& coords) {
        const int index = m_node_index;
        m_map.Bind(coords, index);
        ++m_node_index;
        return index;
    }

    static Standard_Boolean IsEqual(const gp_XYZ& pnt1, const gp_XYZ& pnt2) {
        return (pnt1 - pnt2).SquareModulus() < Precision::SquareConfusion();
    }

    static Standard_Integer HashCode(const gp_XYZ& pnt, Standard_Integer upper) {
        return ::HashCode(pnt.X() * M_LN10 + pnt.Y() * M_PI + pnt.Z() * M_E, upper);
    }

private:
    NCollection_DataMap<gp_XYZ, Standard_Integer, OCC_MergeNodeTool> m_map;
    int m_node_index = 1;
};

//
// Implementation
//

template<typename T> constexpr void OCC_copyCoords(
        Vec3<T>* vec, double x, double y, double z)
{
    vec->x = static_cast<T>(x);
    vec->y = static_cast<T>(y);
    vec->z = static_cast<T>(z);
}

template<typename T> constexpr void OCC_copyCoords(
        Vec3<T>* vec, const gp_XYZ& coords)
{
    OCC_copyCoords(vec, coords.X(), coords.Y(), coords.Z());
}

int OCC_findVertexIndex(
        const gp_XYZ& coords,
        BRepBuilderAPI_CellFilter* filter,
        BRepBuilderAPI_VertexInspector* inspector)
{
    //--------------------------------------------------------------------------
    // Code excerpted from OpenCascade v7.0.0
    //     File: RWStl/RWStl.cxx
    //     Function: "static int AddVertex(...)" lines 38..61
    //--------------------------------------------------------------------------
    inspector->SetCurrent(coords);
    const gp_XYZ min_pnt = inspector->Shift(coords, -Precision::Confusion());
    const gp_XYZ max_pnt = inspector->Shift(coords, +Precision::Confusion());
    filter->Inspect(min_pnt, max_pnt, *inspector);
    if (!inspector->ResInd().IsEmpty()) {
        const int index = inspector->ResInd().First(); // There should be only one
        inspector->ClearResList();
        return index;
    }
    return -1;
}

void OCC_addVertexIndex(
        const gp_XYZ& coords,
        int index,
        BRepBuilderAPI_CellFilter* filter,
        BRepBuilderAPI_VertexInspector* inspector)
{
    filter->Add(index, coords);
    inspector->Add(coords);
}

} // namespace gmio

/*! @} */
