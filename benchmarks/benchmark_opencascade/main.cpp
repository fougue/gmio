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

#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <StlMesh_Mesh.hxx>
#include <Standard_Version.hxx>

#include <gmio_core/error.h>
#include <gmio_core/version.h>
#include <gmio_stl/stl_io.h>
#include <gmio_stl/stl_io_options.h>
#include <gmio_support/stl_occ.h>

#include "../commons/benchmark_tools.h"

#include <iostream>
#include <vector>

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

static void stl_write(const char* filepath, gmio_stl_format format)
{
    gmio_stl_write_options options = {};
    options.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE;
    options.stla_float32_prec = 7;
    gmio_stl_occmesh_iterator occ_itmesh(stlMesh);
    const gmio_stl_mesh occmesh = gmio_stl_occmesh(occ_itmesh);
    const int error = gmio_stl_write_file(format, filepath, &occmesh, &options);
    if (error != GMIO_ERROR_OK)
        std::cerr << "gmio error: 0x" << std::hex << error << std::endl;
}

static void stla_write(const void* filepath)
{
    stl_write(static_cast<const char*>(filepath), GMIO_STL_FORMAT_ASCII);
}

static void stlb_write_le(const void* filepath)
{
    stl_write(static_cast<const char*>(filepath), GMIO_STL_FORMAT_BINARY_LE);
}

static void stlb_write_be(const void* filepath)
{
    stl_write(static_cast<const char*>(filepath), GMIO_STL_FORMAT_BINARY_BE);
}

} // namespace BmkGmio

int main(int argc, char** argv)
{
    if (argc > 1) {
        const char* filepath = argv[1];
        std::cout << std::endl
                  << "gmio v" << GMIO_VERSION_STR << std::endl
                  << "OpenCascade v" << OCC_VERSION_COMPLETE << std::endl
                  << std::endl
                  << "Input file: " << filepath << std::endl;

        /* Declare benchmarks */
        const benchmark_cmp_arg cmp_args[] = {
            { "read",
              BmkGmio::stl_read, filepath,
              BmkOcc::RWStl_ReadFile, filepath },
            { "write(ascii)",
              BmkGmio::stla_write, "__bmk_occ_gmio.stla",
              BmkOcc::RWStl_WriteAscii, "__bmk_occ.stla" },
            { "write(binary/le)",
              BmkGmio::stlb_write_le, "__bmk_occ_gmio.stlb_le",
              BmkOcc::RWStl_WriteBinary, "__bmk_occ.stlb_le" },
            { "write(binary/be)",
              BmkGmio::stlb_write_be, "__bmk_occ_gmio.stlb_be",
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
