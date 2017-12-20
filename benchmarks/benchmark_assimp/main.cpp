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

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/version.h>

#include <gmio_core/error.h>
#include <gmio_core/version.h>
#include <gmio_stl/stl_io.h>
#include <gmio_stl/stl_io_options.h>
#include <gmio_stl/stl_triangle.h>
#include <gmio_stl/stl_mesh.h>
#include <gmio_stl/stl_mesh_creator.h>

#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>

static unsigned totalTriangleCount(const aiScene* scene)
{
    unsigned int meshnum = 0;
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
            ++meshnum;
    }
    return meshnum;
}

inline aiVector3D to_aiVector3D(const gmio::Vec3f& coords) {
    const aiVector3D v = reinterpret_cast<const aiVector3D&>(coords);
    return v;
}

inline gmio::Vec3f to_gmioVec3f(const aiVector3D& vec3) {
    const gmio::Vec3f coords = reinterpret_cast<const gmio::Vec3f&>(vec3);
    return coords;
}

namespace BmkAssimp {

static std::string assimp_version_str()
{
    std::stringstream ss;
    ss << aiGetVersionMajor() << '.' << aiGetVersionMinor() << ".?";
    return ss.str();
}

Assimp::Importer* global_importer = nullptr;
const aiScene* global_scene = nullptr;

static void import(const char* filepath)
{
    Assimp::Importer* importer = global_importer;
    const aiScene* scene = importer->ReadFile(filepath, 0);
    const char* aiErrorStr = aiGetErrorString();
    if (std::strlen(aiErrorStr) > 0)
        std::cerr << aiErrorStr << '\n';
    if (scene == nullptr || scene->mNumMeshes <= 0) {
        std::cerr << "Failed to read file " << filepath << '\n';
    }
    global_scene = scene;
//    std::cout << "BmkAssimp, triCount = "
//              << totalTriangleCount(scene) << std::endl;
}

static void export_stla(const char* filepath)
{
    Assimp::Exporter exporter;
//    for (std::size_t i = 0; i < exporter.GetExportFormatCount(); ++i) {
//        std::cout << exporter.GetExportFormatDescription(i)->id << " "
//                  << exporter.GetExportFormatDescription(i)->description
//                  << std::endl;
//    }
    exporter.Export(global_scene, "stl", filepath);
}

static void export_stlb(const char* filepath)
{
    Assimp::Exporter exporter;
    exporter.Export(global_scene, "stlb", filepath);
}

} // namespace BmkAssimp

namespace BmkGmio {

class STL_MeshCreatorAssimp : public gmio::STL_MeshCreator {
public:
    STL_MeshCreatorAssimp(aiScene* scene)
        : m_scene(scene)
    { }

    void beginSolid(const gmio::STL_MeshCreatorInfos& infos) override
    {
        aiScene* pScene = m_scene;

        // allocate one mesh
        pScene->mNumMeshes = 1;
        pScene->mMeshes = new aiMesh*[1];
        aiMesh* pMesh = pScene->mMeshes[0] = new aiMesh();
        pMesh->mMaterialIndex = 0;

        // allocate a single node
        pScene->mRootNode = new aiNode();
        pScene->mRootNode->mNumMeshes = 1;
        pScene->mRootNode->mMeshes = new unsigned int[1];
        pScene->mRootNode->mMeshes[0] = 0;

        m_loading_ascii = infos.format == gmio::STL_Format_Ascii;
        if (m_loading_ascii) {
            std::strcpy(pScene->mRootNode->mName.data, infos.ascii_solid_name.data());
            pScene->mRootNode->mName.length = infos.ascii_solid_name.size();

            // try to guess how many vertices we could have
            // assume we'll need 160 bytes for each face
            const size_t size_estimate =
                    (static_cast<size_t>(infos.ascii_solid_size) / 160u) * 3;
            m_ascii_vec_vertex.reserve(std::max<size_t>(1, size_estimate));
            m_ascii_vec_normal.reserve(std::max<size_t>(1, size_estimate));
        }
        else {
            pScene->mRootNode->mName.Set("<STL_BINARY>");
            pMesh->mNumFaces = infos.binary_triangle_count;
            pMesh->mNumVertices = pMesh->mNumFaces*3;
            pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
            pMesh->mNormals = new aiVector3D[pMesh->mNumVertices];
        }
    }

    void addTriangle(uint32_t tri_id, const gmio::STL_Triangle& triangle) override
    {
        aiScene* pScene = m_scene;
        aiMesh* pMesh = pScene->mMeshes[0];

        if (m_loading_ascii) {
            const aiVector3D n = to_aiVector3D(triangle.n);
            m_ascii_vec_normal.push_back(n);
            m_ascii_vec_normal.push_back(n);
            m_ascii_vec_normal.push_back(n);

            m_ascii_vec_vertex.push_back(to_aiVector3D(triangle.v1));
            m_ascii_vec_vertex.push_back(to_aiVector3D(triangle.v2));
            m_ascii_vec_vertex.push_back(to_aiVector3D(triangle.v3));
        }
        else {
            aiVector3D* vp = &pMesh->mVertices[tri_id * 3];
            aiVector3D* vn = &pMesh->mNormals[tri_id * 3];

            *vn = to_aiVector3D(triangle.n);
            *(vn+1) = *vn;
            *(vn+2) = *vn;

            *vp = to_aiVector3D(triangle.v1);
            *(vp+1) = to_aiVector3D(triangle.v2);
            *(vp+2) = to_aiVector3D(triangle.v3);
        }
    }

    void endSolid() override
    {
        aiScene* pScene = m_scene;
        aiMesh* pMesh = pScene->mMeshes[0];

        if (m_loading_ascii) {
            if (m_ascii_vec_vertex.empty()
                    || m_ascii_vec_vertex.size() % 3 != 0
                    || m_ascii_vec_normal.size() != m_ascii_vec_vertex.size())
            {
                pMesh->mNumFaces = 0;
                // TODO report error
            }
            else {
                const unsigned ascii_vec_vertex_size =
                        static_cast<unsigned>(m_ascii_vec_vertex.size());
                pMesh->mNumFaces = ascii_vec_vertex_size / 3;
                pMesh->mNumVertices = ascii_vec_vertex_size;
                pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
                std::memcpy(pMesh->mVertices,
                            &m_ascii_vec_vertex[0].x,
                            pMesh->mNumVertices * sizeof(aiVector3D));
                pMesh->mNormals = new aiVector3D[pMesh->mNumVertices];
                std::memcpy(pMesh->mNormals,
                            &m_ascii_vec_normal[0].x,
                            pMesh->mNumVertices * sizeof(aiVector3D));
            }
            m_ascii_vec_vertex.clear();
            m_ascii_vec_normal.clear();
        }
        // now copy faces
        pMesh->mFaces = new aiFace[pMesh->mNumFaces];
        for (unsigned int i = 0, p = 0; i < pMesh->mNumFaces; ++i) {
            aiFace& face = pMesh->mFaces[i];
            face.mIndices = new unsigned int[face.mNumIndices = 3];
            for (unsigned int o = 0; o < 3; ++o,++p)
                face.mIndices[o] = p;
        }

        // create a single default material, using a light gray diffuse color for
        // consistency with other geometric types (e.g., PLY).
        auto pcMat = new aiMaterial;
        aiString s;
        s.Set(AI_DEFAULT_MATERIAL_NAME);
        pcMat->AddProperty(&s, AI_MATKEY_NAME);

        aiColor4D clrDiffuse(0.6f,0.6f,0.6f,1.0f);
        pcMat->AddProperty(&clrDiffuse,1,AI_MATKEY_COLOR_DIFFUSE);
        pcMat->AddProperty(&clrDiffuse,1,AI_MATKEY_COLOR_SPECULAR);
        clrDiffuse = aiColor4D(0.05f,0.05f,0.05f,1.0f);
        pcMat->AddProperty(&clrDiffuse,1,AI_MATKEY_COLOR_AMBIENT);

        pScene->mNumMaterials = 1;
        pScene->mMaterials = new aiMaterial*[1];
        pScene->mMaterials[0] = pcMat;
    }

private:
    aiScene* m_scene;
    bool m_loading_ascii;
    std::vector<aiVector3D> m_ascii_vec_vertex;
    std::vector<aiVector3D> m_ascii_vec_normal;
};

class STL_MeshAssimp : public gmio::STL_Mesh {
public:
    STL_MeshAssimp(const aiMesh* mesh)
        : m_mesh(mesh)
    {
        this->setTriangleCount(mesh->mNumFaces);
    }

    gmio::STL_Triangle triangle(uint32_t tri_id) const override
    {
        const aiFace& f = m_mesh->mFaces[tri_id];

        // we need per-face normals. We specified aiProcess_GenNormals as
        // pre-requisite for this exporter, but nonetheless we have to expect
        // per-vertex normals.
        aiVector3D n;
        if (m_mesh->mNormals) {
            for (unsigned int a = 0; a < f.mNumIndices; ++a)
                n += m_mesh->mNormals[f.mIndices[a]];
            n.Normalize();
        }

        gmio::STL_Triangle tri;
        tri.n = to_gmioVec3f(n);
    #if 0
        tri.n = to_gmioVec3f(m_mesh->mNormals[f.mIndices[0]]);
    #endif
        tri.v1 = to_gmioVec3f(m_mesh->mVertices[f.mIndices[0]]);
        tri.v2 = to_gmioVec3f(m_mesh->mVertices[f.mIndices[1]]);
        tri.v3 = to_gmioVec3f(m_mesh->mVertices[f.mIndices[2]]);
        return tri;
    }

private:
    const aiMesh* m_mesh;
};

aiScene* global_scene = nullptr;

static void stl_read(const char* filepath)
{
    STL_MeshCreatorAssimp creator(global_scene);
    const int error = gmio::STL_read(filepath, &creator);
    if (error != gmio::Error_OK)
        std::cerr << "gmio error: 0x" << std::hex << error << '\n';

//    std::cout << "BmkGmio, triCount = "
//              << totalTriangleCount(global_scene) << std::endl;
}

static void stl_write(const char* filepath, gmio::STL_Format format)
{
    const STL_MeshAssimp mesh(global_scene->mMeshes[0]);
    gmio::STL_WriteOptions opts = {};
    opts.ascii_float32_format = gmio::FloatTextFormat::DecimalLowercase;
    opts.ascii_float32_prec = 7;
    const int error = gmio::STL_write(format, filepath, mesh, opts);
    if (error != gmio::Error_OK)
        std::cerr << "gmio error: 0x" << std::hex << error << '\n';
}

} // namespace BmkGmio

static void bmk_init()
{
    BmkAssimp::global_importer = new Assimp::Importer;
    BmkGmio::global_scene = new aiScene;
}

static void bmk_cleanup()
{
    delete BmkAssimp::global_importer;
    BmkAssimp::global_importer = nullptr;
    delete BmkGmio::global_scene;
    BmkGmio::global_scene = nullptr;
}

int main(int argc, char** argv)
{
    if (argc <= 1)
        return -1;

    const char* filepath = argv[1];
    std::cout << "\ngmio v" << GMIO_VERSION_STR
              << "\nAssimp v" << BmkAssimp::assimp_version_str()
              << "\n\nInput file: " << filepath << '\n';

    const gmio::Benchmark_CmpArg cmp_args[] = {
        { "read",
          [=]{ BmkGmio::stl_read(filepath); },
          [=]{ BmkAssimp::import(filepath); }
        },
        { "write(ascii)",
          []{ BmkGmio::stl_write(
                  "__bmk_assimp_gmio.stla", gmio::STL_Format_Ascii); },
          []{ BmkAssimp::export_stla("__bmk_assimp.stla"); }
        },
        { "write(binary/le)",
          []{ BmkGmio::stl_write(
                  "__bmk_assimp_gmio.stlb_le", gmio::STL_Format_BinaryLittleEndian); },
          []{ BmkAssimp::export_stlb("__bmk_assimp.stlb_le"); }
        },
        { "write(binary/be)",
          []{ BmkGmio::stl_write(
                  "__bmk_assimp_gmio.stlb_be", gmio::STL_Format_BinaryBigEndian); },
          nullptr,
        }
    };
    const std::vector<gmio::Benchmark_CmpResult> results =
            gmio::Benchmark_cmpBatch(
                5, gmio::makeSpan(cmp_args), &bmk_init, &bmk_cleanup);
    gmio::Benchmark_printResults_Markdown(
                std::cout, { "gmio", "Assimp" }, results);
    return 0;
}
