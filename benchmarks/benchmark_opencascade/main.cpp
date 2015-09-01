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

#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>
#include <gmio_support/stl_occ.h>

#include "../commons/benchmark_tools.h"

namespace BenchmarkOcc {

Handle_StlMesh_Mesh stlMesh;

static void bmk_RWStl_ReadFile(const char* filepath)
{
    stlMesh = RWStl::ReadFile(OSD_Path(filepath));
    if (stlMesh.IsNull())
        printf("RWStl::ReadFile(): null mesh\n");
}

static void bmk_RWStl_WriteAscii(const char* filepath)
{
    if (!RWStl::WriteAscii(stlMesh, OSD_Path(filepath)))
        printf("RWStl::WriteAscii() failure\n");
}

static void bmk_RWStl_WriteBinary(const char* filepath)
{
    if (!RWStl::WriteBinary(stlMesh, OSD_Path(filepath)))
        printf("RWStl::WriteBinary() failure\n");
}

static void bmk_main(const char* filepath)
{
    benchmark(BenchmarkOcc::bmk_RWStl_ReadFile,
              "RWStl::ReadFile()",
              filepath);
    benchmark(BenchmarkOcc::bmk_RWStl_WriteAscii,
              "RWStl::WriteAscii",
              "__file_bench_occ.stla");
    benchmark(BenchmarkOcc::bmk_RWStl_WriteBinary,
              "RWStl::WriteBinary",
              "__file_bench_occ.stlb");
}

} // namespace BenchmarkOcc

namespace BenchmarkGmio {

Handle_StlMesh_Mesh stlMesh;

static void bmk_stl_read(const char* filepath)
{
    stlMesh = new StlMesh_Mesh;
    gmio_stl_mesh_creator_t mesh_creator = gmio_stl_occmesh_creator(stlMesh);
    int error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
    if (error != GMIO_ERROR_OK)
        printf("gmio error: 0x%X\n", error);
}

static void bmk_stl_write(const char* filepath, gmio_stl_format_t format)
{
    const gmio_OccStlMeshDomain occMeshDomain(stlMesh);
    const gmio_stl_mesh_t mesh = gmio_stl_occmesh(occMeshDomain);

    gmio_stl_write_options_t opts = { 0 };
    opts.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE;
    opts.stla_float32_prec = 7;
    const int error = gmio_stl_write_file(format, filepath, &mesh, NULL, &opts);
    if (error != GMIO_ERROR_OK)
        printf("gmio error: 0x%X\n", error);
}

static void bmk_stla_write(const char* filepath)
{
    bmk_stl_write(filepath, GMIO_STL_FORMAT_ASCII);
}

static void bmk_stlb_write_le(const char* filepath)
{
    bmk_stl_write(filepath, GMIO_STL_FORMAT_BINARY_LE);
}

static void bmk_stlb_write_be(const char* filepath)
{
    bmk_stl_write(filepath, GMIO_STL_FORMAT_BINARY_BE);
}

static void bmk_main(const char* filepath)
{
    benchmark(bmk_stl_read,
              "gmio_stl_read()",
              filepath);
    benchmark(bmk_stla_write,
              "gmio_stl_write(STL_ASCII)",
              "__file_bench_gmio.stla");
    benchmark(bmk_stlb_write_le,
              "gmio_stl_write(STL_BINARY_LE)",
              "__file_bench_gmio_le.stlb");
    benchmark(bmk_stlb_write_be,
              "gmio_stl_write(STL_BINARY_BE)",
              "__file_bench_gmio_be.stlb");
}
} // namespace BenchmarkGmio

int main(int argc, char** argv)
{
    if (argc > 1) {
        benchmark_forward_list(BenchmarkOcc::bmk_main, argc - 1, argv + 1);
        benchmark_forward_list(BenchmarkGmio::bmk_main, argc - 1, argv + 1);
    }

    return 0;
}
