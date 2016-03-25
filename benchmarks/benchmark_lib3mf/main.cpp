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

#include "../commons/benchmark_tools.h"

#include <Common/Math/NMR_Matrix.h>
#include <Common/Math/NMR_VectorTree.h>
#include <Common/MeshExport/NMR_MeshExporter_STL.h>
#include <Common/MeshImport/NMR_MeshImporter_STL.h>
#include <Common/Platform/NMR_ExportStream_COM.h>
#include <Common/Platform/NMR_ImportStream_COM.h>

#include <gmio_core/error.h>
#include <gmio_core/memblock.h>
#include <gmio_core/version.h>
#include <gmio_stl/stl_format.h>
#include <gmio_stl/stl_io.h>
#include <gmio_stl/stl_infos.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <windows.h>

namespace BmkLib3MF {

NMR::CMesh globalMesh;

std::wstring multibyte_to_wstring(const char* str, int codepage = 0)
{
    const int str_len = static_cast<int>(std::strlen(str));
    const int wlen = MultiByteToWideChar(codepage, 0, str, str_len, nullptr, 0);
    std::wstring wstr;
    wstr.resize(wlen + 1);
    const int wstr_len = static_cast<int>(wstr.size());
    MultiByteToWideChar(codepage, 0, str, str_len, &wstr[0], wstr_len);
    return wstr;
}

static void import(const void* filepath)
{
    const char* cstr_filepath = static_cast<const char*>(filepath);
    const std::wstring wstr_filepath = multibyte_to_wstring(cstr_filepath);
    auto istream = std::make_shared<NMR::CImportStream_COM>(wstr_filepath.c_str());
    NMR::CMeshImporter_STL importer(istream);
    importer.setImportColors(false);
    importer.setIgnoreInvalidFaces(true);
    try {
        importer.loadMesh(&globalMesh, nullptr);
        if (GetLastError() != ERROR_SUCCESS)
            std::cerr << "NMR::CMeshImporter_STL error" << std::endl;
    }
    catch(...) {
        std::cerr << "NMR::CMeshImporter_STL::loadMesh() exception" << std::endl;
    }
}

static void export_binary(const void* filepath)
{
    const char* cstr_filepath = static_cast<const char*>(filepath);
    const std::wstring wstr_filepath = multibyte_to_wstring(cstr_filepath);
    auto ostream = std::make_shared<NMR::CExportStream_COM>(wstr_filepath.c_str());
    NMR::CMeshExporter_STL exporter(ostream);
    exporter.exportMesh(&globalMesh, nullptr);
    if (GetLastError() != ERROR_SUCCESS)
        std::cerr << "NMR::CMeshExporter_STL error" << std::endl;
}

} // namespace BmkLib3MF

namespace BmkGmio {

struct NMR_CMeshHelper
{
    NMR::CMesh mesh;
    const NMR::NMATRIX3* matrix;
    NMR::CVectorTree vectorTree;
    bool ignoreInvalidFaces;
};

NMR_CMeshHelper globalMeshHelper;

static void get_triangle(
        const void* cookie, uint32_t tri_id, gmio_stl_triangle* triangle)
{
    // Note: code from lib3mf/Source/Common/MeshImport/NMR_MeshImporter_STL.cpp
    // Commit #a466df4

    auto constMeshHelper = static_cast<const NMR_CMeshHelper*>(cookie);
    auto meshHelper = const_cast<NMR_CMeshHelper*>(constMeshHelper);
    NMR::CMesh* pMesh = &meshHelper->mesh;
    const NMR::NMATRIX3* pmMatrix = meshHelper->matrix;
    NMR::MESHFACE* face = pMesh->getFace(tri_id);
    NMR::MESHFORMAT_STL_FACET facet;

    for (int j = 0; j < 3; j++) {
        NMR::MESHNODE* node = pMesh->getNode(face->m_nodeindices[j]);
        if (pmMatrix)
            facet.m_vertieces[j] = NMR::fnMATRIX3_apply(*pmMatrix, node->m_position);
        else
            facet.m_vertieces[j] = node->m_position;
    }

    // Calculate Triangle Normals
    const NMR::NVEC3 n =
            NMR::fnVEC3_calcTriangleNormal(
                facet.m_vertieces[0], facet.m_vertieces[1], facet.m_vertieces[2]);
    std::memcpy(&triangle->n, &n, sizeof(gmio_vec3f));
    std::memcpy(&triangle->v1, &facet.m_vertieces[0], sizeof(gmio_vec3f));
    std::memcpy(&triangle->v2, &facet.m_vertieces[1], sizeof(gmio_vec3f));
    std::memcpy(&triangle->v3, &facet.m_vertieces[2], sizeof(gmio_vec3f));
    triangle->attribute_byte_count = 0;
}

static void add_triangle(
        void* cookie, uint32_t /*tri_id*/, const gmio_stl_triangle* triangle)
{
    // Note: code from lib3mf/Source/Common/MeshExport/NMR_MeshExporter_STL.cpp
    // Commit #a466df4

    auto meshHelper = static_cast<NMR_CMeshHelper*>(cookie);
    NMR::CMesh* pMesh = &meshHelper->mesh;
    const NMR::NMATRIX3* pmMatrix = meshHelper->matrix;
    NMR::CVectorTree& VectorTree = meshHelper->vectorTree;
    NMR::MESHNODE* pNodes[3];

    // Check, if Coordinates are in Valid Space
    bool bIsValid = true;
    {
        const gmio_vec3f* vertex_ptr = &triangle->v1;
        for (int i = 0; i < 3; i++) {
            const float* coord_ptr = &vertex_ptr[i].x;
            for (int j = 0; j < 3; j++)
                bIsValid &= (fabs(coord_ptr[j]) < NMR_MESH_MAXCOORDINATE);
        }
    }

    // Identify Nodes via Tree
    if (bIsValid) {
        const gmio_vec3f* vertex_ptr = &triangle->v1;
        for (int j = 0; j < 3; j++) {
            NMR::NVEC3 vPosition;
            std::memcpy(&vPosition, &vertex_ptr[j], sizeof(NMR::NVEC3));
            if (pmMatrix)
                vPosition = NMR::fnMATRIX3_apply(*pmMatrix, vPosition);

            uint32_t nNodeIdx;
            if (VectorTree.findVector3(vPosition, nNodeIdx)) {
                pNodes[j] = pMesh->getNode(nNodeIdx);
            }
            else {
                pNodes[j] = pMesh->addNode(vPosition);
                VectorTree.addVector3(
                            pNodes[j]->m_position,
                            (uint32_t)pNodes[j]->m_index);
            }
        }

        // check, if Nodes are separate
        bIsValid = (pNodes[0] != pNodes[1])
                && (pNodes[0] != pNodes[2])
                && (pNodes[1] != pNodes[2]);
    }

    // Throw "Invalid Exception"
    if ((!bIsValid) && !meshHelper->ignoreInvalidFaces)
        throw NMR::CNMRException(NMR_ERROR_INVALIDCOORDINATES);

    if (bIsValid)
        pMesh->addFace(pNodes[0], pNodes[1], pNodes[2]);
}

static void stl_read(const void* filepath)
{
    const char* str_filepath = static_cast<const char*>(filepath);
    gmio_stl_mesh_creator mesh_creator = {};
    mesh_creator.cookie = &globalMeshHelper;
    mesh_creator.func_add_triangle = add_triangle;
    const int error = gmio_stl_read_file(str_filepath, &mesh_creator, NULL);
    if (error != GMIO_ERROR_OK)
        std::cout << "gmio error: 0x" << std::hex << error << std::endl;
}

static void stl_write_generic(const char* filepath, gmio_stl_format format)
{
    gmio_stl_mesh mesh = {};
    mesh.cookie = &globalMeshHelper;
    mesh.triangle_count = globalMeshHelper.mesh.getFaceCount();
    mesh.func_get_triangle = get_triangle;

    gmio_stl_write_options opts = {};
    opts.stla_solid_name = filepath;
    opts.stla_float32_prec = 7;
    opts.stlb_header = gmio_stlb_header_str(filepath);
    const int error = gmio_stl_write_file(format, filepath, &mesh, &opts);
    if (error != GMIO_ERROR_OK)
        std::cout << "gmio error: 0x" << std::hex << error << std::endl;
}

static void stla_write(const void* filepath)
{
    stl_write_generic(
                static_cast<const char*>(filepath), GMIO_STL_FORMAT_ASCII);
}

static void stlb_write_le(const void* filepath)
{
    stl_write_generic(
                static_cast<const char*>(filepath), GMIO_STL_FORMAT_BINARY_LE);
}

static void stlb_write_be(const void* filepath)
{
    stl_write_generic(
                static_cast<const char*>(filepath), GMIO_STL_FORMAT_BINARY_BE);
}

} // namespace BmkGmio

static void bmk_init()
{
    BmkLib3MF::globalMesh.clear();
    BmkGmio::globalMeshHelper.mesh.clear();
    BmkGmio::globalMeshHelper.vectorTree = NMR::CVectorTree();
    BmkGmio::globalMeshHelper.matrix = nullptr;
    BmkGmio::globalMeshHelper.ignoreInvalidFaces = true;
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        const char* filepath = argv[1];
        std::cout << std::endl
                  << "gmio v" << GMIO_VERSION_STR << std::endl
                  << "Lib3MF commit a466df47231" << std::endl
                  << std::endl
                  << "Input file: " << filepath << std::endl;

        /* Declare benchmarks */
        const benchmark_cmp_arg cmp_args[] = {
            { "read",
              BmkGmio::stl_read, filepath,
              BmkLib3MF::import, filepath },
            { "write(ascii)",
              BmkGmio::stla_write, "__bmk_lib3mf_gmio.stla",
              nullptr, nullptr },
            { "write(binary/le)",
              BmkGmio::stlb_write_le, "__bmk_lib3mf_gmio.stlb_le",
              BmkLib3MF::export_binary, "__bmk_lib3mf.stlb_le" },
            { "write(binary/be)",
              BmkGmio::stlb_write_be, "__bmk_lib3mf_gmio.stlb_be",
              nullptr, nullptr },
            {}
        };

        /* Execute benchmarks */
        std::vector<benchmark_cmp_result> cmp_res_vec;
        cmp_res_vec.resize(GMIO_ARRAY_SIZE(cmp_args) - 1);
        benchmark_cmp_batch(
                    5, cmp_args, &cmp_res_vec[0], bmk_init, nullptr);

        /* Print results */
        const benchmark_cmp_result_array res_array = {
            &cmp_res_vec.at(0), cmp_res_vec.size() };
        const benchmark_cmp_result_header header = { "gmio", "Lib3MF" };
        benchmark_print_results(
                    BENCHMARK_PRINT_FORMAT_MARKDOWN, header, res_array);
    }
    return 0;
}
