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

#include "../commons/benchmark_tools.h"

#include <Common/Math/NMR_Matrix.h>
#include <Common/Math/NMR_VectorTree.h>
#include <Common/MeshExport/NMR_MeshExporter_STL.h>
#include <Common/MeshImport/NMR_MeshImporter_STL.h>
#include <Common/Platform/NMR_ExportStream_COM.h>
#include <Common/Platform/NMR_ImportStream_COM.h>

#include <gmio_core/error.h>
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

NMR::CMesh global_mesh;

static std::wstring multibyte_to_wstring(const char* str, int codepage = 0)
{
    const int str_len = static_cast<int>(std::strlen(str));
    const int wlen = MultiByteToWideChar(codepage, 0, str, str_len, nullptr, 0);
    std::wstring wstr;
    wstr.resize(wlen + 1);
    const int wstr_len = static_cast<int>(wstr.size());
    MultiByteToWideChar(codepage, 0, str, str_len, &wstr[0], wstr_len);
    return wstr;
}

static void import(const char* filepath)
{
    const std::wstring wstr_filepath = multibyte_to_wstring(filepath);
    auto istream = std::make_shared<NMR::CImportStream_COM>(wstr_filepath.c_str());
    NMR::CMeshImporter_STL importer(istream);
    importer.setImportColors(false);
    importer.setIgnoreInvalidFaces(true);
    try {
        importer.loadMesh(&global_mesh, nullptr);
        if (GetLastError() != ERROR_SUCCESS)
            std::cerr << "NMR::CMeshImporter_STL error\n";
    }
    catch(...) {
        std::cerr << "NMR::CMeshImporter_STL::loadMesh() exception\n";
    }
}

static void export_binary(const char* filepath)
{
    const std::wstring wstr_filepath = multibyte_to_wstring(filepath);
    auto ostream = std::make_shared<NMR::CExportStream_COM>(wstr_filepath.c_str());
    NMR::CMeshExporter_STL exporter(ostream);
    exporter.exportMesh(&global_mesh, nullptr);
    if (GetLastError() != ERROR_SUCCESS)
        std::cerr << "NMR::CMeshExporter_STL error" << std::endl;
}

} // namespace BmkLib3MF

namespace BmkGmio {

struct NMR_CMeshHelper
{
    NMR::CMesh mesh;
    const NMR::NMATRIX3* matrix;
    NMR::CVectorTree vector_tree;
    bool ignore_invalid_faces;
};

NMR_CMeshHelper global_mesh_helper;

class STL_Mesh3MF : public gmio::STL_Mesh {
public:
    STL_Mesh3MF(const NMR_CMeshHelper* mesh_helper)
        : m_mesh_helper(mesh_helper)
    {
        this->setTriangleCount(
                    const_cast<NMR::CMesh*>(&mesh_helper->mesh)->getFaceCount());
    }

    gmio::STL_Triangle triangle(uint32_t tri_id) const override
    {
        // Note: code from lib3mf/Source/Common/MeshImport/NMR_MeshImporter_STL.cpp
        // Commit #a466df4

        NMR::CMesh* pMesh = const_cast<NMR::CMesh*>(&m_mesh_helper->mesh);
        const NMR::NMATRIX3* pmMatrix = m_mesh_helper->matrix;
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
        gmio::STL_Triangle triangle;
        std::memcpy(&triangle.n, &n, sizeof(gmio::Vec3f));
        std::memcpy(&triangle.v1, &facet.m_vertieces[0], sizeof(gmio::Vec3f));
        std::memcpy(&triangle.v2, &facet.m_vertieces[1], sizeof(gmio::Vec3f));
        std::memcpy(&triangle.v3, &facet.m_vertieces[2], sizeof(gmio::Vec3f));
        triangle.attribute_byte_count = 0;
        return triangle;
    }

private:
    const NMR_CMeshHelper* m_mesh_helper;
};

class STL_MeshCreator3MF : public gmio::STL_MeshCreator {
public:
    STL_MeshCreator3MF(NMR_CMeshHelper* mesh_helper)
        : m_mesh_helper(mesh_helper)
    { }

    void addTriangle(uint32_t, const gmio::STL_Triangle& triangle) override
    {
        // Note: code from lib3mf/Source/Common/MeshExport/NMR_MeshExporter_STL.cpp
        // Commit #a466df4

        NMR::CMesh* pMesh = &m_mesh_helper->mesh;
        const NMR::NMATRIX3* pmMatrix = m_mesh_helper->matrix;
        NMR::CVectorTree& VectorTree = m_mesh_helper->vector_tree;
        NMR::MESHNODE* pNodes[3];

        // Check, if Coordinates are in Valid Space
        bool bIsValid = true;
        {
            const gmio::Vec3f* vertex_ptr = &triangle.v1;
            for (int i = 0; i < 3; i++) {
                const float* coord_ptr = &vertex_ptr[i].x;
                for (int j = 0; j < 3; j++)
                    bIsValid &= (std::fabs(coord_ptr[j]) < NMR_MESH_MAXCOORDINATE);
            }
        }

        // Identify Nodes via Tree
        if (bIsValid) {
            const gmio::Vec3f* vertex_ptr = &triangle.v1;
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
        if ((!bIsValid) && !m_mesh_helper->ignore_invalid_faces)
            throw NMR::CNMRException(NMR_ERROR_INVALIDCOORDINATES);

        if (bIsValid)
            pMesh->addFace(pNodes[0], pNodes[1], pNodes[2]);
    }

private:
    NMR_CMeshHelper* m_mesh_helper;
};

static void stl_read(const char* filepath)
{
    STL_MeshCreator3MF mesh_creator(&global_mesh_helper);
    const int error = gmio::STL_read(filepath, &mesh_creator);
    if (error != gmio::Error_OK)
        std::cout << "gmio error: 0x" << std::hex << error << '\n';
}

static void stl_write(const char* filepath, gmio::STL_Format format)
{
    const STL_Mesh3MF mesh(&global_mesh_helper);
    gmio::STL_WriteOptions opts = {};
    opts.ascii_solid_name = filepath;
    opts.ascii_float32_prec = 7;
    opts.binary_header =
            gmio::STL_binaryHeaderFromString(
                gmio::makeSpan(filepath, std::strlen(filepath)));
    const int error = gmio::STL_write(format, filepath, mesh, opts);
    if (error != gmio::Error_OK)
        std::cout << "gmio error: 0x" << std::hex << error << '\n';
}

} // namespace BmkGmio

static void bmk_init()
{
    BmkLib3MF::global_mesh.clear();
    BmkGmio::global_mesh_helper.mesh.clear();
    BmkGmio::global_mesh_helper.vector_tree = NMR::CVectorTree();
    BmkGmio::global_mesh_helper.matrix = nullptr;
    BmkGmio::global_mesh_helper.ignore_invalid_faces = true;
}

int main(int argc, char** argv)
{
    if (argc <= 1)
        return -1;

    const char* filepath = argv[1];
    std::cout << "\ngmio v" << GMIO_VERSION_STR
              << "\nLib3MF commit a466df47231"
              << "\n\nInput file: " << filepath << '\n';

    const gmio::Benchmark_CmpArg cmp_args[] = {
        { "read",
          [=]{ BmkGmio::stl_read(filepath); },
          [=]{ BmkLib3MF::import(filepath); }
        },
        { "write(ascii)",
          []{ BmkGmio::stl_write(
                  "__bmk_lib3mf_gmio.stla", gmio::STL_Format_Ascii); },
          nullptr
        },
        { "write(binary/le)",
          []{ BmkGmio::stl_write(
                  "__bmk_lib3mf_gmio.stlb_le", gmio::STL_Format_BinaryLittleEndian); },
          []{ BmkLib3MF::export_binary("__bmk_lib3mf.stlb_le"); }
        },
        { "write(binary/be)",
          []{ BmkGmio::stl_write(
                  "__bmk_lib3mf_gmio.stlb_be", gmio::STL_Format_BinaryBigEndian); },
          nullptr
        }
    };
    const std::vector<gmio::Benchmark_CmpResult> results =
            gmio::Benchmark_cmpBatch(
                5, gmio::makeSpan(cmp_args), &bmk_init);
    gmio::Benchmark_printResults_Markdown(
                std::cout, { "gmio", "Lib3MF" }, results);
    return 0;
}
