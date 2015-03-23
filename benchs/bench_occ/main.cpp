#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlAPI_Reader.hxx>

#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>
#include <gmio_support/occ_libstl.h>

#include "../commons/bench_tools.h"

static void bench_occ_RWStl_ReadFile(const char* filepath)
{
  RWStl::ReadFile(OSD_Path(filepath));
}

static void bench_gmio_stl_read(const char* filepath)
{
//    void* mbuffer = std::malloc(512 * 1024);
//    gmio_buffer_t buffer = gmio_buffer(mbuffer, 512 * 1024);
    char mbuffer[256 * 1024];
    gmio_buffer_t buffer = gmio_buffer(&mbuffer[0], sizeof(mbuffer));

    Handle_StlMesh_Mesh mesh = new StlMesh_Mesh;
    gmio_stl_mesh_creator_t mesh_creator = gmio_stl_occmesh_creator(mesh);

    int error = gmio_stl_read_file(filepath, &mesh_creator, &buffer);
    if (error != GMIO_NO_ERROR)
        printf("GeomIO error: 0x%X\n", error);

    //std::free(mbuffer);
}

int main(int argc, char** argv)
{
  if (argc > 1) {
    benchmark(&bench_occ_RWStl_ReadFile,
              "RWStl::ReadFile()",
              argc - 1, argv + 1);

    benchmark(&bench_gmio_stl_read,
              "gmio_stl_read()",
              argc - 1, argv + 1);
  }

  return 0;
}
