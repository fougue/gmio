#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>

#include "../commons/bench_tools.h"

typedef struct my_igeom
{
    uint32_t facet_count;
} my_igeom_t;

static void dummy_process_triangle(void* cookie,
                                   uint32_t triangle_id,
                                   const gmio_stl_triangle_t* triangle)
{
    my_igeom_t* my_igeom = (my_igeom_t*)(cookie);
    if (my_igeom != NULL)
        ++(my_igeom->facet_count);
}

static void bench_gmio_stl_read(const char* filepath)
{
    my_igeom_t cookie = { 0 };
    gmio_stl_mesh_creator_t mesh_creator = { 0 };
    int error = GMIO_ERROR_OK;

    mesh_creator.cookie = &cookie;
    mesh_creator.add_triangle_func = dummy_process_triangle;
    error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
    if (error != GMIO_ERROR_OK)
        printf("GeomIO error: 0x%X\n", error);
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        benchmark(&bench_gmio_stl_read,
                  "gmio_stl_read_file()",
                  argc - 1, argv + 1);
    }
    return 0;
}
