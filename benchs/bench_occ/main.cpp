#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <StlMesh_Mesh.hxx>

#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>
#include <gmio_support/stl_occ.h>

#include "../commons/bench_tools.h"

static void bench_occ_RWStl_ReadFile(const char* filepath)
{
    RWStl::ReadFile(OSD_Path(filepath));
}

static void bench_gmio_stl_read(const char* filepath)
{
    Handle_StlMesh_Mesh mesh = new StlMesh_Mesh;
    gmio_stl_mesh_creator_t mesh_creator = gmio_stl_occmesh_creator(mesh);
    int error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
    if (error != GMIO_ERROR_OK)
        printf("GeomIO error: 0x%X\n", error);
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        benchmark_list(&bench_occ_RWStl_ReadFile,
                       "RWStl::ReadFile()",
                       argc - 1, argv + 1);

        benchmark_list(&bench_gmio_stl_read,
                       "gmio_stl_read()",
                       argc - 1, argv + 1);
    }

    return 0;
}
