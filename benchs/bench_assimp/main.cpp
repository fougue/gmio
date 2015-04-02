#include "../commons/bench_tools.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/cimport.h>

#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>

#include <cstring>
#include <iostream>

static void bench_assimp_Importer(const char* filepath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath, 0);
    if (aiGetErrorString() != NULL)
        std::cerr << aiGetErrorString() << std::endl;
    if (scene == NULL || scene->mNumMeshes <= 0) {
        std::cerr << "Failed to read file " << filepath << std::endl;
    }
}

static void gmio_assimp_allocate_stl_scene(aiScene* pScene)
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

static void gmio_assimp_ascii_begin_solid_func(
        void* cookie, size_t stream_size, const char* solid_name)
{
    aiScene* pScene = (aiScene*)cookie;
    gmio_assimp_allocate_stl_scene(pScene);
    aiMesh* pMesh = pScene->mMeshes[0];

    std::strcpy(pScene->mRootNode->mName.data, solid_name);
    pScene->mRootNode->mName.length = std::strlen(solid_name);

    // try to guess how many vertices we could have
    // assume we'll need 160 bytes for each face
    const unsigned facetSize = 200u;
    pMesh->mNumFaces = std::max(1u, static_cast<unsigned>(stream_size) / facetSize);
    pMesh->mNumVertices = pMesh->mNumFaces * 3;
    pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
    pMesh->mNormals  = new aiVector3D[pMesh->mNumVertices];
}

static void gmio_assimp_binary_begin_solid(
        void* cookie, uint32_t tri_count, const uint8_t* /*header*/)
{
    aiScene* pScene = (aiScene*)cookie;
    gmio_assimp_allocate_stl_scene(pScene);
    aiMesh* pMesh = pScene->mMeshes[0];

    pScene->mRootNode->mName.Set("<STL_BINARY>");
    pMesh->mNumFaces = tri_count;

    pMesh->mNumVertices = pMesh->mNumFaces*3;
    pMesh->mVertices = new aiVector3D[pMesh->mNumVertices];
    pMesh->mNormals = new aiVector3D[pMesh->mNumVertices];
}

static void gmio_assimp_add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle_t* triangle)
{
    aiScene* pScene = (aiScene*)cookie;
    aiMesh* pMesh = pScene->mMeshes[0];
    if (pMesh->mNumFaces <= tri_id) {
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
    aiVector3D* vn = &pMesh->mNormals[tri_id];

    *vn = *((aiVector3D*)&triangle->normal);
    *vp++ = *((aiVector3D*)&triangle->v1);
    *vp++ = *((aiVector3D*)&triangle->v2);
    *vp++ = *((aiVector3D*)&triangle->v3);
}

static void gmio_assimp_end_solid(void* cookie)
{
    aiScene* pScene = (aiScene*)cookie;
    aiMesh* pMesh = pScene->mMeshes[0];

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

static void bench_gmio_stl_read(const char* filepath)
{
    gmio_buffer_t buffer = gmio_buffer_malloc(256 * 1024);

    aiScene* scene = new aiScene;
    gmio_stl_mesh_creator_t mesh_creator = { 0 };
    mesh_creator.cookie = scene;
    mesh_creator.ascii_begin_solid_func = gmio_assimp_ascii_begin_solid_func;
    mesh_creator.binary_begin_solid_func = gmio_assimp_binary_begin_solid;
    mesh_creator.add_triangle_func = gmio_assimp_add_triangle;
    mesh_creator.end_solid_func = gmio_assimp_end_solid;

    int error = gmio_stl_read_file(filepath, &buffer, &mesh_creator);
    if (error != GMIO_ERROR_OK)
        printf("GeomIO error: 0x%X\n", error);

    gmio_buffer_deallocate(&buffer);
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        benchmark(&bench_assimp_Importer,
                  "Assimp::Importer::ReadFile()",
                  argc - 1, argv + 1);
        benchmark(&bench_gmio_stl_read,
                  "gmio_stl_read()",
                  argc - 1, argv + 1);
    }
    return 0;
}
