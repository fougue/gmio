#include "../commons/bench_tools.h"

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>

#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>

#include <cstring>
#include <iostream>

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
        aiVector3D* vec3, const gmio_stl_coords_t& coords)
{
    *vec3 = *((aiVector3D*)&coords);
}

GMIO_INLINE void copy_aiVector3D(
        gmio_stl_coords_t* coords, const aiVector3D& vec3)
{
    *coords = *((gmio_stl_coords_t*)&vec3);
}

namespace BenchmarkAssimp {

Assimp::Importer* globalImporter = NULL;
const aiScene* globalScene = NULL;

static void bmk_import(const char* filepath)
{
    Assimp::Importer* importer = globalImporter;
    const aiScene* scene = importer->ReadFile(filepath, 0);
    if (aiGetErrorString() != NULL)
        std::cerr << aiGetErrorString() << std::endl;
    if (scene == NULL || scene->mNumMeshes <= 0) {
        std::cerr << "Failed to read file " << filepath << std::endl;
    }
    globalScene = scene;
    std::cout << "BenchAssimp, triCount = "
              << totalTriangleCount(scene) << std::endl;
}

static void bmk_export_stla(const char* filepath)
{
    Assimp::Exporter exporter;
//    for (std::size_t i = 0; i < exporter.GetExportFormatCount(); ++i) {
//        std::cout << exporter.GetExportFormatDescription(i)->id << " "
//                  << exporter.GetExportFormatDescription(i)->description
//                  << std::endl;
//    }
    exporter.Export(globalScene, "stl", filepath);
}

static void bmk_export_stlb(const char* filepath)
{
    Assimp::Exporter exporter;
    exporter.Export(globalScene, "stlb", filepath);
}

} // namespace BenchAssimp

static void bench_assimp(const char* filepath)
{
    BenchmarkAssimp::globalImporter = new Assimp::Importer;
    benchmark(BenchmarkAssimp::bmk_import,
              "Assimp::Importer::ReadFile()",
              filepath);
    benchmark(BenchmarkAssimp::bmk_export_stla,
              "Assimp::Exporter::Export(STL_ASCII)",
              "__file_bench_assimp.stla");
    benchmark(BenchmarkAssimp::bmk_export_stlb,
              "Assimp::Exporter::Export(STL_BINARY)",
              "__file_bench_assimp.stlb");

    delete BenchmarkAssimp::globalImporter;
    BenchmarkAssimp::globalImporter = NULL;
}

static void bench_assimp_list(int fileCount, char** files)
{
    for (int i = 0; i < fileCount; ++i)
        bench_assimp(files[i]);
}

namespace BenchmarkGmio {

struct aiSceneHelper
{
    aiScene* scene;
    uint32_t totalTriangleCount;
    int hasToCountTriangle;
};

aiSceneHelper globalSceneHelper = { 0 };

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

static void ascii_begin_solid_func(
        void* cookie, size_t stream_size, const char* solid_name)
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
    pMesh->mNumFaces = std::max(1u, static_cast<unsigned>(stream_size) / facetSize);
    pMesh->mNumVertices = pMesh->mNumFaces * 3;
    pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
    pMesh->mNormals  = new aiVector3D[pMesh->mNumVertices];
}

static void binary_begin_solid(
        void* cookie, uint32_t tri_count, const uint8_t* /*header*/)
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
        void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* triangle)
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

    // create a single default material, using a light gray diffuse color for consistency with
    // other geometric types (e.g., PLY).
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
        const void* cookie, uint32_t tri_id, gmio_stl_triangle_t* triangle)
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

static void bmk_stl_read(const char* filepath)
{
    gmio_stl_mesh_creator_t mesh_creator = { 0 };
    mesh_creator.cookie = &globalSceneHelper;
    mesh_creator.ascii_begin_solid_func = ascii_begin_solid_func;
    mesh_creator.binary_begin_solid_func = binary_begin_solid;
    mesh_creator.add_triangle_func = add_triangle;
    mesh_creator.end_solid_func = end_solid;

    const int error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
    if (error != GMIO_ERROR_OK)
        printf("GeomIO error: 0x%X\n", error);

    const aiScene* scene = globalSceneHelper.scene;
    std::cout << "BenchGmio, triCount = "
              << totalTriangleCount(scene) << std::endl;
}

static void bmk_stl_write(const char* filepath, gmio_stl_format_t format)
{
    const aiMesh* sceneMesh = globalSceneHelper.scene->mMeshes[0];

    gmio_stl_mesh_t mesh = { 0 };
    mesh.cookie = sceneMesh;
    mesh.triangle_count = sceneMesh->mNumFaces;
    mesh.get_triangle_func = get_triangle;

    gmio_stl_write_options_t opts = { 0 };
    opts.stla_float32_format = GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE;
    opts.stla_float32_prec = 7;
    const int error = gmio_stl_write_file(format, filepath, &mesh, NULL, &opts);
    if (error != GMIO_ERROR_OK)
        printf("GeomIO error: 0x%X\n", error);
}

static void bmk_stla_write(const char* filepath)
{
    bmk_stl_write(filepath, GMIO_STL_FORMAT_ASCII);
}

static void bmk_stlb_write(const char* filepath)
{
    bmk_stl_write(filepath, GMIO_STL_FORMAT_BINARY_LE);
}

} // namespace BenchmarkGmio

static void bench_gmio(const char* filepath)
{
    BenchmarkGmio::globalSceneHelper.scene = new aiScene;
    BenchmarkGmio::globalSceneHelper.totalTriangleCount = 0;

    benchmark(BenchmarkGmio::bmk_stl_read,
              "gmio_stl_read()",
              filepath);
    benchmark(BenchmarkGmio::bmk_stla_write,
              "gmio_stl_write(STL_ASCII)",
              "__file_bench_gmio.stla");
    benchmark(BenchmarkGmio::bmk_stlb_write,
              "gmio_stl_write(STL_BINARY)",
              "__file_bench_gmio.stlb");

    delete BenchmarkGmio::globalSceneHelper.scene;
    BenchmarkGmio::globalSceneHelper.scene = NULL;
}

static void bench_gmio_list(int fileCount, char** files)
{
    for (int i = 0; i < fileCount; ++i)
        bench_gmio(files[i]);
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        bench_assimp_list(argc - 1, argv + 1);
        bench_gmio_list(argc - 1, argv + 1);
    }
    return 0;
}
