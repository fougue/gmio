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

#include <gmio_core/error.h>
#include <gmio_core/version.h>
#include <gmio_stl/stl_io.h>
#include <gmio_stl/stl_io_options.h>
#include <gmio_support/stl_occ_brep.h>
#include <gmio_support/stl_occ_polytri.h>

#include <Standard_Version.hxx>
#if OCC_VERSION_HEX < 0x070200
#  include <gmio_support/stl_occ_mesh.h>
#  include <StlMesh_Mesh.hxx>
#endif

#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <IGESControl_Reader.hxx>
#include <Message_ProgressIndicator.hxx>
#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <StlAPI_Writer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_WorkSession.hxx>

#include <STEPControl_Reader.hxx>

#include "../commons/benchmark_tools.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

namespace BmkBRep {

TopoDS_Shape input_shape;

class ProgressIndicator : public Message_ProgressIndicator {
public:
    Standard_Boolean Show(const Standard_Boolean /*force*/) override
    {
        const Standard_Real pc = this->GetPosition(); // Always within [0,1]
        const int val = static_cast<int>(1 + pc * (100 - 1));
        if (val > m_val) {
            std::cout << val;
            if (val < 100)
                std::cout << "-";
            else
                std::cout << "%";
            std::cout.flush();
            m_val = val;
        }
        return Standard_True;
    }

    Standard_Boolean UserBreak() override
    { return Standard_False; }

private:
    int m_val = 0;
};

template<typename READER>
TopoDS_Shape loadShapeFromFile(
        const char* fileName,
        Handle_Message_ProgressIndicator indicator)
{
    TopoDS_Shape result;

    if (!indicator.IsNull())
        indicator->NewScope(30, "Loading file");
    READER reader;
    const int status = reader.ReadFile(const_cast<Standard_CString>(fileName));
    if (!indicator.IsNull())
        indicator->EndScope();
    if (status == IFSelect_RetDone) {
        if (!indicator.IsNull()) {
            reader.WS()->MapReader()->SetProgress(indicator);
            indicator->NewScope(70, "Translating file");
        }
        reader.NbRootsForTransfer();
        reader.TransferRoots();
        result = reader.OneShape();
        if (!indicator.IsNull()) {
            indicator->EndScope();
            reader.WS()->MapReader()->SetProgress(NULL);
        }
    }
    return result;
}

void readInputIgesShape(const char* filepath)
{
    Handle_Message_ProgressIndicator indicator = new ProgressIndicator;
    input_shape = loadShapeFromFile<IGESControl_Reader>(filepath, indicator);
}

void readInputStepShape(const char* filepath)
{
    Handle_Message_ProgressIndicator indicator = new ProgressIndicator;
    input_shape = loadShapeFromFile<STEPControl_Reader>(filepath, indicator);
}

} // namespace BmkBRep

namespace BmkOcc {

Handle_Poly_Triangulation global_polytri;

static void RWStl_ReadFile(const char* filepath)
{
    global_polytri = RWStl::ReadFile(OSD_Path(filepath));
    if (global_polytri.IsNull())
        std::cerr << "RWStl::ReadFile(): null mesh\n";
}

static void RWStl_WriteAscii(const char* filepath)
{
    if (!RWStl::WriteAscii(global_polytri, OSD_Path(filepath)))
        std::cerr << "RWStl::WriteAscii() failure\n";
}

static void RWStl_WriteBinary(const char* filepath)
{
    if (!RWStl::WriteBinary(global_polytri, OSD_Path(filepath)))
        std::cerr << "RWStl::WriteBinary() failure\n";
}

static void StlAPI_WriteAscii(const char* filepath)
{
    if (!BmkBRep::input_shape.IsNull()) {
        StlAPI_Writer writer;
        writer.ASCIIMode() = Standard_True;
        writer.Write(BmkBRep::input_shape, filepath);
    }
}

static void StlAPI_WriteBinary(const char* filepath)
{
    if (!BmkBRep::input_shape.IsNull()) {
        StlAPI_Writer writer;
        writer.ASCIIMode() = Standard_False;
        writer.Write(BmkBRep::input_shape, filepath);
    }
}

} // namespace BmkOcc

namespace BmkGmio {

Handle_Poly_Triangulation global_polytri;

static void stl_read(const char* filepath)
{
    gmio::STL_MeshCreatorOccPolyTriangulation creator;
    const int error = gmio::STL_read(filepath, &creator);
    global_polytri = creator.polytri();
    if (error != gmio::Error_OK)
        std::cerr << "gmio error: 0x" << std::hex << error << '\n';
}

static void stl_write(
        const char* filepath, gmio::STL_Format format, const gmio::STL_Mesh& mesh)
{
    gmio::STL_WriteOptions options = {};
    //options.ascii_float32_format = gmio::FloatTextFormat::ShortestUppercase;
    options.ascii_float32_format = gmio::FloatTextFormat::ScientificLowercase;
    options.ascii_float32_prec = 6;
    const int error = gmio::STL_write(format, filepath, mesh, options);
    if (error != gmio::Error_OK)
        std::cerr << "gmio error: 0x" << std::hex << error << '\n';
}

static void stl_mesh_write(const char* filepath, gmio::STL_Format format)
{
    const gmio::STL_MeshOccPolyTriangulation mesh(global_polytri);
    stl_write(filepath, format, mesh);
}

static void stl_brep_write(const char* filepath, gmio::STL_Format format)
{
    const gmio::STL_MeshOccShape mesh(BmkBRep::input_shape);
    stl_write(filepath, format, mesh);
}

} // namespace BmkGmio

int main(int argc, char** argv)
{
    const char* stl_filepath = nullptr;
    const char* igs_filepath = nullptr;
    const char* stp_filepath = nullptr;
    double linear_deflection = 0.1;
    int iarg = 1;
    while (iarg < argc) {
        if (std::strcmp(argv[iarg], "--iges") == 0)
            igs_filepath = argv[++iarg];
        else if (std::strcmp(argv[iarg], "--step") == 0)
            stp_filepath = argv[++iarg];
        else if (std::strcmp(argv[iarg], "--linear-deflection") == 0)
            linear_deflection = std::atof(argv[++iarg]);
        else
            stl_filepath = argv[iarg];
        ++iarg;
    }

    if (stl_filepath != nullptr) {
        std::cout << "\ngmio v" << GMIO_VERSION_STR
                  << "\nOpenCascade v" << OCC_VERSION_COMPLETE
                  << "\n\nSTL input file:  " << stl_filepath << '\n';

        if (igs_filepath != nullptr) {
            std::cout << "IGES input file: " << igs_filepath << '\n';
            BmkBRep::readInputIgesShape(igs_filepath);
        }
        else if (stp_filepath != nullptr) {
            std::cout << "STEP input file: " << stp_filepath << '\n';
            BmkBRep::readInputStepShape(stp_filepath);
        }

        std::cout << "\nMeshing with linear deflection="
                  << linear_deflection
                  << " ...\n";
        for (TopExp_Explorer expl(BmkBRep::input_shape, TopAbs_FACE);
             expl.More();
             expl.Next())
        {
            const TopoDS_Face& face = TopoDS::Face(expl.Current());
            TopLoc_Location location;
            const auto& poly = BRep_Tool::Triangulation(face, location);
            if (poly.IsNull() || poly->Triangles().Length() <= 0)
                BRepMesh_IncrementalMesh(face, linear_deflection);
        }
        std::cout << "\nMeshing done\n";

        const gmio::Benchmark_CmpArg cmp_args[] = {
            { "read",
              [=]{ BmkGmio::stl_read(stl_filepath); },
              [=]{ BmkOcc::RWStl_ReadFile(stl_filepath); }
            },
            { "meshwrite(ascii)",
              []{ BmkGmio::stl_mesh_write(
                      "__bmk_occ_gmio_mesh.stla", gmio::STL_Format_Ascii); },
              []{ BmkOcc::RWStl_WriteAscii("__bmk_occ_mesh.stla"); }
            },
            { "meshwrite(binary/le)",
              []{ BmkGmio::stl_mesh_write(
                      "__bmk_occ_gmio_mesh.stlb_le", gmio::STL_Format_BinaryLittleEndian); },
              []{ BmkOcc::RWStl_WriteBinary("__bmk_occ_mesh.stlb_le"); }
            },
            { "meshwrite(binary/be)",
              []{ BmkGmio::stl_mesh_write(
                      "__bmk_occ_gmio_mesh.stlb_be", gmio::STL_Format_BinaryBigEndian); },
              nullptr
            },
            { "brepwrite(ascii)",
              []{ BmkGmio::stl_brep_write(
                      "__bmk_occ_gmio_brep.stla", gmio::STL_Format_Ascii); },
              []{ BmkOcc::StlAPI_WriteAscii("__bmk_occ_brep.stla"); }
            },
            { "brepwrite(binary/le)",
              []{ BmkGmio::stl_brep_write(
                      "__bmk_occ_gmio_brep.stlb_le", gmio::STL_Format_BinaryLittleEndian); },
              []{ BmkOcc::StlAPI_WriteBinary("__bmk_occ_brep.stlb_le"); }
            },
            { "brepwrite(binary/be)",
              []{ BmkGmio::stl_brep_write(
                      "__bmk_occ_gmio_brep.stlb_be", gmio::STL_Format_BinaryBigEndian); },
              nullptr
            }
        };
        const std::vector<gmio::Benchmark_CmpResult> results =
                gmio::Benchmark_cmpBatch(5, gmio::makeSpan(cmp_args));
        gmio::Benchmark_printResults_Markdown(
                    std::cout, { "gmio", "OpenCascade" }, results);
    }
    return 0;
}
