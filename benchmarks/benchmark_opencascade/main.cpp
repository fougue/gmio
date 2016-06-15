/****************************************************************************
** gmio benchmarks
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software provides performance benchmarks for the gmio library
** (https://github.com/fougue/gmio)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <IGESControl_Reader.hxx>
#include <Message_ProgressIndicator.hxx>
#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <Standard_Version.hxx>
#include <StlAPI_Writer.hxx>
#include <StlMesh_Mesh.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_WorkSession.hxx>

#include <STEPControl_Reader.hxx>

#include <gmio_core/error.h>
#include <gmio_core/version.h>
#include <gmio_stl/stl_io.h>
#include <gmio_stl/stl_io_options.h>
#include <gmio_support/stl_occ_brep.h>
#include <gmio_support/stl_occ_mesh.h>

#include "../commons/benchmark_tools.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

namespace BmkBRep {

TopoDS_Shape inputShape;

class ProgressIndicator : public Message_ProgressIndicator
{
public:
    Standard_Boolean Show(const Standard_Boolean /*force*/) override
    {
        const Standard_Real pc = this->GetPosition(); // Always within [0,1]
        const int val = 1 + pc * (100 - 1);
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
    inputShape = loadShapeFromFile<IGESControl_Reader>(filepath, indicator);
}

void readInputStepShape(const char* filepath)
{
    Handle_Message_ProgressIndicator indicator = new ProgressIndicator;
    inputShape = loadShapeFromFile<STEPControl_Reader>(filepath, indicator);
}

} // namespace BmkBRep

namespace BmkOcc {

Handle_StlMesh_Mesh stlMesh;

static void RWStl_ReadFile(const void* filepath)
{
    stlMesh = RWStl::ReadFile(OSD_Path(static_cast<const char*>(filepath)));
    if (stlMesh.IsNull())
        std::cerr << "RWStl::ReadFile(): null mesh" << std::endl;
}

static void RWStl_WriteAscii(const void* filepath)
{
    if (!RWStl::WriteAscii(stlMesh, OSD_Path(static_cast<const char*>(filepath))))
        std::cerr << "RWStl::WriteAscii() failure" << std::endl;
}

static void RWStl_WriteBinary(const void* filepath)
{
    if (!RWStl::WriteBinary(stlMesh, OSD_Path(static_cast<const char*>(filepath))))
        std::cerr << "RWStl::WriteBinary() failure" << std::endl;
}

static void StlAPI_WriteAscii(const void* filepath)
{
    StlAPI_Writer writer;
    writer.ASCIIMode() = Standard_True;
    const char* cfilepath = static_cast<const char*>(filepath);
    const StlAPI_ErrorStatus err = writer.Write(BmkBRep::inputShape, cfilepath);
    if (err != StlAPI_StatusOK)
        std::cerr << "StlAPI_Writer::Write() error: " << err << std::endl;
}

static void StlAPI_WriteBinary(const void* filepath)
{
    StlAPI_Writer writer;
    writer.ASCIIMode() = Standard_False;
    const char* cfilepath = static_cast<const char*>(filepath);
    const StlAPI_ErrorStatus err = writer.Write(BmkBRep::inputShape, cfilepath);
    if (err != StlAPI_StatusOK)
        std::cerr << "StlAPI_Writer::Write() error: " << err << std::endl;
}

} // namespace BmkOcc

namespace BmkGmio {

Handle_StlMesh_Mesh stlMesh;

static void stl_read(const void* filepath)
{
    stlMesh = new StlMesh_Mesh;
    gmio_stl_mesh_creator mesh_creator = gmio_stl_occmesh_creator(stlMesh);
    const int error = gmio_stl_read_file(
                static_cast<const char*>(filepath), &mesh_creator, NULL);
    if (error != GMIO_ERROR_OK)
        std::cerr << "gmio error: 0x" << std::hex << error << std::endl;
}

static void stl_write(
        const char* filepath, gmio_stl_format format, const gmio_stl_mesh& mesh)
{
    gmio_stl_write_options options = {};
    //options.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE;
    options.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_LOWERCASE;
    options.stla_float32_prec = 6;
    const int error = gmio_stl_write_file(format, filepath, &mesh, &options);
    if (error != GMIO_ERROR_OK)
        std::cerr << "gmio error: 0x" << std::hex << error << std::endl;
}

static void stl_mesh_write(const char* filepath, gmio_stl_format format)
{
    const gmio_stl_mesh_occmesh mesh(stlMesh);
    stl_write(filepath, format, mesh);
}

static void stl_brep_write(const char* filepath, gmio_stl_format format)
{
    const gmio_stl_mesh_occshape mesh(BmkBRep::inputShape);
    stl_write(filepath, format, mesh);
}

static void stla_mesh_write(const void* filepath)
{
    stl_mesh_write(static_cast<const char*>(filepath), GMIO_STL_FORMAT_ASCII);
}

static void stlb_mesh_write_le(const void* filepath)
{
    stl_mesh_write(static_cast<const char*>(filepath), GMIO_STL_FORMAT_BINARY_LE);
}

static void stlb_mesh_write_be(const void* filepath)
{
    stl_mesh_write(static_cast<const char*>(filepath), GMIO_STL_FORMAT_BINARY_BE);
}

static void stla_brep_write(const void* filepath)
{
    stl_brep_write(static_cast<const char*>(filepath), GMIO_STL_FORMAT_ASCII);
}

static void stlb_brep_write_le(const void* filepath)
{
    stl_brep_write(static_cast<const char*>(filepath), GMIO_STL_FORMAT_BINARY_LE);
}

static void stlb_brep_write_be(const void* filepath)
{
    stl_brep_write(static_cast<const char*>(filepath), GMIO_STL_FORMAT_BINARY_BE);
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
        std::cout << std::endl
                  << "gmio v" << GMIO_VERSION_STR << std::endl
                  << "OpenCascade v" << OCC_VERSION_COMPLETE << std::endl
                  << std::endl
                  << "STL input file:  " << stl_filepath << std::endl;

        if (igs_filepath != nullptr) {
            std::cout << "IGES input file: " << igs_filepath << std::endl;
            BmkBRep::readInputIgesShape(igs_filepath);
        }
        else if (stp_filepath != nullptr) {
            std::cout << "STEP input file: " << stp_filepath << std::endl;
            BmkBRep::readInputStepShape(stp_filepath);
        }

        std::cout << std::endl << "Meshing with linear deflection="
                  << linear_deflection
                  << " ..." << std::endl;
        for (TopExp_Explorer expl(BmkBRep::inputShape, TopAbs_FACE);
             expl.More();
             expl.Next())
        {
            const TopoDS_Face& face = TopoDS::Face(expl.Current());
            TopLoc_Location location;
            const auto& poly = BRep_Tool::Triangulation(face, location);
            if (poly.IsNull() || poly->Triangles().Length() <= 0)
                BRepMesh_IncrementalMesh(face, linear_deflection);
        }
        std::cout << std::endl << "Meshing done" << std::endl;

        /* Declare benchmarks */
        const benchmark_cmp_arg cmp_args[] = {
            { "read",
              BmkGmio::stl_read, stl_filepath,
              BmkOcc::RWStl_ReadFile, stl_filepath },
            { "meshwrite(ascii)",
              BmkGmio::stla_mesh_write, "__bmk_occ_gmio_mesh.stla",
              BmkOcc::RWStl_WriteAscii, "__bmk_occ_mesh.stla" },
            { "meshwrite(binary/le)",
              BmkGmio::stlb_mesh_write_le, "__bmk_occ_gmio_mesh.stlb_le",
              BmkOcc::RWStl_WriteBinary, "__bmk_occ_mesh.stlb_le" },
            { "meshwrite(binary/be)",
              BmkGmio::stlb_mesh_write_be, "__bmk_occ_gmio_mesh.stlb_be",
              NULL, NULL },
            { "brepwrite(ascii)",
              BmkGmio::stla_brep_write, "__bmk_occ_gmio_brep.stla",
              BmkOcc::StlAPI_WriteAscii, "__bmk_occ_brep.stla" },
            { "brepwrite(binary/le)",
              BmkGmio::stlb_brep_write_le, "__bmk_occ_gmio_brep.stlb_le",
              BmkOcc::StlAPI_WriteBinary, "__bmk_occ_brep.stlb_le" },
            { "brepwrite(binary/be)",
              BmkGmio::stlb_brep_write_be, "__bmk_occ_gmio_brep.stlb_be",
              NULL, NULL },
            {}
        };

        /* Execute benchmarks */
        std::vector<benchmark_cmp_result> cmp_res_vec;
        cmp_res_vec.resize(GMIO_ARRAY_SIZE(cmp_args) - 1);
        benchmark_cmp_batch(5, cmp_args, &cmp_res_vec[0], NULL, NULL);

        /* Print results */
        const benchmark_cmp_result_array res_array = {
            &cmp_res_vec.at(0), cmp_res_vec.size() };
        const benchmark_cmp_result_header header = {
            "gmio", "OpenCascade" };
        benchmark_print_results(
                    BENCHMARK_PRINT_FORMAT_MARKDOWN, header, res_array);
    }
    return 0;
}
