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

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/version.h>

#include <gmio_core/error.h>
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

GMIO_INLINE void copy_gmio_stl_coords(
        aiVector3D* vec3, const gmio_stl_coords& coords)
{
    *vec3 = *((aiVector3D*)&coords);
}

GMIO_INLINE void copy_aiVector3D(
        gmio_stl_coords* coords, const aiVector3D& vec3)
{
    *coords = *((gmio_stl_coords*)&vec3);
}

namespace BmkAssimp {

static std::string assimp_version_str()
{
    std::stringstream ss;
    ss << "Assimp v"
       << aiGetVersionMajor() << '.'
       << aiGetVersionMinor()
       << ".?";
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

static void func_ascii_begin_solid(
        void* cookie, gmio_streamsize_t stream_size, const char* solid_name)
{
    aiSceneHelper* helper = (aiSceneHelper*)cookie;
    helper->hasToCountTriangle = 1; // true
    aiScene* pScene = helper->scene;
    allocate_stl_scene(pScene);
    aiMesh* pMesh = pScene->mMeshes[0];

    std::strcpy(pScene->mRootNode->mName.data, solid_name);
    pScene->mRootNode->mName.length = std::strlen(solid_name);

    // try to guess how many vertices we could have
    // assume we'll need 200 bytes for each face
    const unsigned facetSize = 200u;
    pMesh->mNumFaces =
            std::max(1u, static_cast<unsigned>(stream_size) / facetSize);
    pMesh->mNumVertices = pMesh->mNumFaces * 3;
    pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
    pMesh->mNormals  = new aiVector3D[pMesh->mNumVertices];
}

static void binary_begin_solid(
        void* cookie, uint32_t tri_count, const gmio_stlb_header* /*header*/)
{
    aiSceneHelper* helper = (aiSceneHelper*)cookie;
    helper->hasToCountTriangle = 0; // false
    aiScene* pScene = helper->scene;
    allocate_stl_scene(pScene);
    aiMesh* pMesh = pScene->mMeshes[0];

    pScene->mRootNode->mName.Set("<STL_BINARY>");
    pMesh->mNumFaces = tri_count;

    pMesh->mNumVertices = pMesh->mNumFaces*3;
    pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
    pMesh->mNormals = new aiVector3D[pMesh->mNumVertices];
}

static void add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle* triangle)
{
    aiSceneHelper* helper = (aiSceneHelper*)cookie;
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

    copy_gmio_stl_coords(vn, triangle->normal);
    *(vn+1) = *vn;
    *(vn+2) = *vn;

    copy_gmio_stl_coords(vp, triangle->v1);
    copy_gmio_stl_coords(vp+1, triangle->v2);
    copy_gmio_stl_coords(vp+2, triangle->v3);

    if (helper->hasToCountTriangle)
        ++(helper->totalTriangleCount);
}

static void end_solid(void* cookie)
{
    aiSceneHelper* helper = (aiSceneHelper*)cookie;
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
    const aiMesh* mesh = (const aiMesh*)cookie;
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
    copy_aiVector3D(&triangle->normal, nor);
#if 0
    copy_aiVector3D(&triangle->normal, mesh->mNormals[f.mIndices[0]]);
#endif

    copy_aiVector3D(&triangle->v1, mesh->mVertices[f.mIndices[0]]);
    copy_aiVector3D(&triangle->v2, mesh->mVertices[f.mIndices[1]]);
    copy_aiVector3D(&triangle->v3, mesh->mVertices[f.mIndices[2]]);
}

static void stl_read(const void* filepath)
{
    gmio_stl_read_args read = {};
    read.mesh_creator.cookie = &globalSceneHelper;
    read.mesh_creator.func_ascii_begin_solid = func_ascii_begin_solid;
    read.mesh_creator.func_binary_begin_solid = binary_begin_solid;
    read.mesh_creator.func_add_triangle = add_triangle;
    read.mesh_creator.func_end_solid = end_solid;

    const int error =
            gmio_stl_read_file(&read, static_cast<const char*>(filepath));
    if (error != GMIO_ERROR_OK)
        printf("gmio error: 0x%X\n", error);

    const aiScene* scene = globalSceneHelper.scene;
//    std::cout << "BmkGmio, triCount = "
//              << totalTriangleCount(scene) << std::endl;
}

static void stl_write(const char* filepath, gmio_stl_format format)
{
    const aiMesh* sceneMesh = globalSceneHelper.scene->mMeshes[0];

    gmio_stl_write_args write = {};
    write.mesh.cookie = sceneMesh;
    write.mesh.triangle_count = sceneMesh->mNumFaces;
    write.mesh.func_get_triangle = get_triangle;
    write.options.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE;
    write.options.stla_float32_prec = 7;
    const int error = gmio_stl_write_file(&write, format, filepath);
    if (error != GMIO_ERROR_OK)
        printf("gmio error: 0x%X\n", error);
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
        std::cout << std::endl << "Input file: " << filepath << std::endl;

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
                    5, &cmp_args[0], &cmp_res_vec[0], bmk_init, bmk_cleanup);

        /* Print results */
        const benchmark_cmp_result_array res_array = {
            &cmp_res_vec.at(0), cmp_res_vec.size() };
        const std::string assimp_ver = BmkAssimp::assimp_version_str();
        const benchmark_cmp_result_header header = { "gmio", assimp_ver.c_str() };
        benchmark_print_results(
                    BENCHMARK_PRINT_FORMAT_MARKDOWN, header, res_array);
    }
    return 0;
}
