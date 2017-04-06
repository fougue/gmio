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

GMIO_INLINE void gmio_stl_occ_copy_xyz(
        gmio_vec3f* vec, double x, double y, double z);

GMIO_INLINE void gmio_stl_occ_copy_xyz(
        gmio_vec3f* vec, const gp_XYZ& coords);

GMIO_INLINE int gmio_occ_find_vertex_index(
        const gp_XYZ& coords,
        BRepBuilderAPI_CellFilter* filter,
        BRepBuilderAPI_VertexInspector* inspector);

GMIO_INLINE void gmio_occ_add_vertex_index(
        const gp_XYZ& coords,
        int index,
        BRepBuilderAPI_CellFilter* filter,
        BRepBuilderAPI_VertexInspector* inspector);



/*
 * Implementation
 */

void gmio_stl_occ_copy_xyz(gmio_vec3f* vec, double x, double y, double z)
{
    vec->x = static_cast<float>(x);
    vec->y = static_cast<float>(y);
    vec->z = static_cast<float>(z);
}

void gmio_stl_occ_copy_xyz(gmio_vec3f* vec, const gp_XYZ& coords)
{
    gmio_stl_occ_copy_xyz(vec, coords.X(), coords.Y(), coords.Z());
}

int gmio_occ_find_vertex_index(
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

void gmio_occ_add_vertex_index(
        const gp_XYZ& coords,
        int index,
        BRepBuilderAPI_CellFilter* filter,
        BRepBuilderAPI_VertexInspector* inspector)
{
    filter->Add(index, coords);
    inspector->Add(coords);
}

/*! @} */
