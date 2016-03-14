/* -----------------------------------------------------------------------------
 *
 * Example: write a STL file
 *
 * Just give the filepath and an initialized gmio_stl_mesh object to
 * gmio_stl_write_file().
 * The gmio_stl_mesh object defines the interface to access the underlying user
 * mesh.
 *
 * Note if you want to have control over the stream to be used, call
 * gmio_stl_write() instead.
 *
 * -------------------------------------------------------------------------- */

#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>
#include <stdio.h>
#include <stdlib.h>
#include "3d_mesh.h"

/* Callback invoked sequentially for each triangle in the STL mesh
 * Just transfer the triangle of index 'tri_id' into 'triangle' */
static void my_3d_mesh__get_triangle(
        const void* cookie, uint32_t tri_id, struct gmio_stl_triangle* triangle)
{
    const struct my_3d_mesh* my_mesh = (const struct my_3d_mesh*)cookie;
    const struct my_3d_triangle* my_tri = &my_mesh->triangle_array[tri_id];
    struct gmio_vec3_f32* tri_vertices = &triangle->v1;
    int i;
    for (i = 0; i < 3; ++i) {
        tri_vertices[i].x = (float)my_tri->vertex[i].coords[0];
        tri_vertices[i].y = (float)my_tri->vertex[i].coords[1];
        tri_vertices[i].z = (float)my_tri->vertex[i].coords[2];
    }
    gmio_stl_triangle_compute_normal(triangle);
}

int main(int argc, char** argv)
{
    int error = 0;
    if (argc > 1) {
        const char* filepath = argv[1];
        struct my_3d_mesh my_mesh = {0};
        struct gmio_stl_mesh mesh = {0};

        /* Construct the user mesh */
        /* ... */

        /* Initialize the mesh interface */
        mesh.cookie = &my_mesh;
        mesh.triangle_count = my_mesh.triangle_array_count;
        mesh.func_get_triangle = my_3d_mesh__get_triangle;

        /* Write binary STL little-endian, using default options(NULL) */
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_LE, filepath, &mesh, NULL);
        if (error != GMIO_ERROR_OK)
            fprintf(stderr, "gmio error: 0x%X\n", error);

        free(my_mesh.triangle_array);
    }
    return 0;
}
