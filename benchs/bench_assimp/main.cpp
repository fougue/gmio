#include <assimp/IOSystem.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/scene.h>

extern "C" {
#include "../commons/bench_tools.h"
}

#include <iostream>

static void assimp_Importer(const char* filepath)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filepath, 0);
  if (scene == NULL || scene->mNumMeshes <= 0) {
    std::cerr << "Failed to read file " << filepath << std::endl;
  }
}

int main(int argc, char** argv)
{
  if (argc > 1)
    benchmark(&assimp_Importer, "Assimp::Importer::ReadFile()", argc - 1, argv + 1);
  return 0;
}
