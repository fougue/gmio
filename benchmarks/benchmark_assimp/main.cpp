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

GMIO_INLINE void copy_gmio_vec3f(aiVector3D* vec3, const gmio_vec3f& coords)
{
    *vec3 = *((aiVector3D*)&coords);
}

GMIO_INLINE void copy_aiVector3D(gmio_vec3f* coords, const aiVector3D& vec3)
{
    *coords = *((gmio_vec3f*)&vec3);
}

namespace BmkAssimp {

static std::string assimp_version_str()
{
    std::stringstream ss;
    ss << aiGetVersionMajor() << '.' << aiGetVersionMinor() << ".?";
    return ss.str();
}

Assimp::Importer* globalImporter = NULL;
const aiScene* globalScene = NULL;

static void import(const void* filepath)
{
    Assimp::Importer* importer = globalImporter;
    const aiScene* scene =
            importer->ReadFile(static_cast<const char*>(filepath), 0);
    const char* aiErrorStr = aiGetErrorString();
    if (std::strlen(aiErrorStr) > 0)
        std::cerr << aiErrorStr << std::endl;
    if (scene == NULL || scene->mNumMeshes <= 0) {
        std::cerr << "Failed to read file " << filepath << std::endl;
    }
    globalScene = scene;
//    std::cout << "BmkAssimp, triCount = "
//              << totalTriangleCount(scene) << std::endl;
}

static void export_stla(const void* filepath)
{
    Assimp::Exporter exporter;
//    for (std::size_t i = 0; i < exporter.GetExportFormatCount(); ++i) {
//        std::cout << exporter.GetExportFormatDescription(i)->id << " "
//                  << exporter.GetExportFormatDescription(i)->description
//                  << std::endl;
//    }
    exporter.Export(globalScene, "stl", static_cast<const char*>(filepath));
}

static void export_stlb(const void* filepath)
{
    Assimp::Exporter exporter;
    exporter.Export(globalScene, "stlb", static_cast<const char*>(filepath));
}

} // namespace BmkAssimp

namespace BmkGmio {

struct aiSceneHelper
{
    aiScene* scene;
    uint32_t totalTriangleCount;
    int hasToCountTriangle;
};

aiSceneHelper globalSceneHelper = {};

static void allocate_stl_scene(aiScene* pScene)
{
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
}

static void func_begin_solid(
        void* cookie, const struct gmio_stl_mesh_creator_infos* infos)
{
    aiSceneHelper* helper = static_cast<aiSceneHelper*>(cookie);

    aiScene* pScene = helper->scene;
    allocate_stl_scene(pScene);
    aiMesh* pMesh = pScene->mMeshes[0];

    if (infos->format == GMIO_STL_FORMAT_ASCII) {
        helper->hasToCountTriangle = 1; // true

        std::strcpy(pScene->mRootNode->mName.data, infos->stla_solid_name);
        pScene->mRootNode->mName.length = std::strlen(infos->stla_solid_name);

        // try to guess how many vertices we could have
        // assume we'll need 200 bytes for each face
        const unsigned estimatedFacetCount =
                static_cast<unsigned>(infos->stla_stream_size) / 200u;
        pMesh->mNumFaces = std::max(1u, estimatedFacetCount);
    }
    else {
        helper->hasToCountTriangle = 0; // false
        pScene->mRootNode->mName.Set("<STL_BINARY>");
        pMesh->mNumFaces = infos->stlb_triangle_count;
    }
    pMesh->mNumVertices = pMesh->mNumFaces*3;
    pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
    pMesh->mNormals = new aiVector3D[pMesh->mNumVertices];
}

static void add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle* triangle)
{
    aiSceneHelper* helper = static_cast<aiSceneHelper*>(cookie);
    aiScene* pScene = helper->scene;
    aiMesh* pMesh = pScene->mMeshes[0];
    if (pMesh->mNumFaces <= tri_id) {
        std::cout << "add_triangle() reallocate" << std::endl;
        // need to resize the arrays, our size estimate was wrong
#if 0
        unsigned int iNeededSize = (unsigned int)(sz-mBuffer) / pMesh->mNumFaces;
        if (iNeededSize <= 160)
            iNeededSize >>= 1; // prevent endless looping
        unsigned int add = (unsigned int)((mBuffer+fileSize)-sz) / iNeededSize;
#endif

        unsigned int add = pMesh->mNumFaces;
        add += add >> 3; // add 12.5% as buffer
        const unsigned int iNeededSize = (pMesh->mNumFaces + add)*3;

        aiVector3D* pv = new aiVector3D[iNeededSize];
        memcpy(pv, pMesh->mVertices, pMesh->mNumVertices*sizeof(aiVector3D));
        delete[] pMesh->mVertices;
        pMesh->mVertices = pv;
        pv = new aiVector3D[iNeededSize];
        memcpy(pv, pMesh->mNormals, pMesh->mNumVertices*sizeof(aiVector3D));
        delete[] pMesh->mNormals;
        pMesh->mNormals = pv;

        pMesh->mNumVertices = iNeededSize;
        pMesh->mNumFaces += add;
    }

    aiVector3D* vp = &pMesh->mVertices[tri_id * 3];
    aiVector3D* vn = &pMesh->mNormals[tri_id * 3];

    copy_gmio_vec3f(vn, triangle->n);
    *(vn+1) = *vn;
    *(vn+2) = *vn;

    copy_gmio_vec3f(vp, triangle->v1);
    copy_gmio_vec3f(vp+1, triangle->v2);
    copy_gmio_vec3f(vp+2, triangle->v3);

    if (helper->hasToCountTriangle)
        ++(helper->totalTriangleCount);
}

static void end_solid(void* cookie)
{
    aiSceneHelper* helper = static_cast<aiSceneHelper*>(cookie);
    aiScene* pScene = helper->scene;
    aiMesh* pMesh = pScene->mMeshes[0];

    if (helper->hasToCountTriangle) {
        pMesh->mNumFaces = helper->totalTriangleCount;
        pMesh->mNumVertices = helper->totalTriangleCount * 3;
    }

    // now copy faces
    pMesh->mFaces = new aiFace[pMesh->mNumFaces];
    for (unsigned int i = 0, p = 0; i < pMesh->mNumFaces;++i)	{
        aiFace& face = pMesh->mFaces[i];
        face.mIndices = new unsigned int[face.mNumIndices = 3];
        for (unsigned int o = 0; o < 3;++o,++p) {
            face.mIndices[o] = p;
        }
    }

    // create a single default material, using a light gray diffuse color for
    // consistency with other geometric types (e.g., PLY).
    aiMaterial* pcMat = new aiMaterial;
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

static void get_triangle(
        const void* cookie, uint32_t tri_id, gmio_stl_triangle* triangle)
{
    const aiMesh* mesh = static_cast<const aiMesh*>(cookie);
    const aiFace& f = mesh->mFaces[tri_id];

    // we need per-face normals. We specified aiProcess_GenNormals as
    // pre-requisite for this exporter, but nonetheless we have to expect
    // per-vertex normals.
    aiVector3D nor;
    if (mesh->mNormals) {
        for (unsigned int a = 0; a < f.mNumIndices; ++a) {
            nor += mesh->mNormals[f.mIndices[a]];
        }
        nor.Normalize();
    }
    copy_aiVector3D(&triangle->n, nor);
#if 0
    copy_aiVector3D(&triangle->n, mesh->mNormals[f.mIndices[0]]);
#endif

    copy_aiVector3D(&triangle->v1, mesh->mVertices[f.mIndices[0]]);
    copy_aiVector3D(&triangle->v2, mesh->mVertices[f.mIndices[1]]);
    copy_aiVector3D(&triangle->v3, mesh->mVertices[f.mIndices[2]]);
}

static void stl_read(const void* filepath)
{
    const char* str_filepath = static_cast<const char*>(filepath);
    gmio_stl_mesh_creator mesh_creator = {};
    mesh_creator.cookie = &globalSceneHelper;
    mesh_creator.func_begin_solid = func_begin_solid;
    mesh_creator.func_add_triangle = add_triangle;
    mesh_creator.func_end_solid = end_solid;

    const int error = gmio_stl_read_file(str_filepath, &mesh_creator, NULL);
    if (error != GMIO_ERROR_OK)
        std::cerr << "gmio error: 0x" << std::hex << error << std::endl;

//    const aiScene* scene = globalSceneHelper.scene;
//    std::cout << "BmkGmio, triCount = "
//              << totalTriangleCount(scene) << std::endl;
}

static void stl_write(const char* filepath, gmio_stl_format format)
{
    const aiMesh* sceneMesh = globalSceneHelper.scene->mMeshes[0];

    gmio_stl_mesh mesh = {};
    mesh.cookie = sceneMesh;
    mesh.triangle_count = sceneMesh->mNumFaces;
    mesh.func_get_triangle = get_triangle;

    gmio_stl_write_options opts = {};
    opts.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE;
    opts.stla_float32_prec = 7;
    const int error = gmio_stl_write_file(format, filepath, &mesh, &opts);
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

static void bmk_init()
{
    BmkAssimp::globalImporter = new Assimp::Importer;
    BmkGmio::globalSceneHelper.scene = new aiScene;
    BmkGmio::globalSceneHelper.totalTriangleCount = 0;
}

static void bmk_cleanup()
{
    delete BmkAssimp::globalImporter;
    BmkAssimp::globalImporter = NULL;
    delete BmkGmio::globalSceneHelper.scene;
    BmkGmio::globalSceneHelper.scene = NULL;
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        const char* filepath = argv[1];
        std::cout << std::endl
                  << "gmio v" << GMIO_VERSION_STR << std::endl
                  << "Assimp v" << BmkAssimp::assimp_version_str() << std::endl
                  << std::endl
                  << "Input file: " << filepath << std::endl;

        /* Declare benchmarks */
        const benchmark_cmp_arg cmp_args[] = {
            { "read",
              BmkGmio::stl_read, filepath,
              BmkAssimp::import, filepath },
            { "write(ascii)",
              BmkGmio::stla_write, "__bmk_assimp_gmio.stla",
              BmkAssimp::export_stla, "__bmk_assimp.stla" },
            { "write(binary/le)",
              BmkGmio::stlb_write_le, "__bmk_assimp_gmio.stlb_le",
              BmkAssimp::export_stlb, "__bmk_assimp.stlb_le" },
            { "write(binary/be)",
              BmkGmio::stlb_write_be, "__bmk_assimp_gmio.stlb_be",
              NULL, NULL },
            {}
        };

        /* Execute benchmarks */
        std::vector<benchmark_cmp_result> cmp_res_vec;
        cmp_res_vec.resize(GMIO_ARRAY_SIZE(cmp_args) - 1);
        benchmark_cmp_batch(
                    5, cmp_args, &cmp_res_vec[0], bmk_init, bmk_cleanup);

        /* Print results */
        const benchmark_cmp_result_array res_array = {
            &cmp_res_vec.at(0), cmp_res_vec.size() };
        const benchmark_cmp_result_header header = {
            "gmio", "Assimp" };
        benchmark_print_results(
                    BENCHMARK_PRINT_FORMAT_MARKDOWN, header, res_array);
    }
    return 0;
}
