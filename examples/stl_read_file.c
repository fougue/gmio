/* -----------------------------------------------------------------------------
 *
 * Example: read a STL file
 *
 * Just give a filepath and an initialized gmio_stl_mesh_creator object to
 * gmio_stl_read_file().
 * The gmio_stl_mesh_creator object holds pointers to the callbacks invoked
 * during the read operation.
 * These callbacks creates the final mesh object.
 *
 * Note if you want to have control over the stream to be used, call
 * gmio_stl_read() instead.
 *
 * -------------------------------------------------------------------------- */

#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>
#include <stdio.h>
#include <stdlib.h>
#include "3d_mesh.h"

/* Callback invoked at the beginning of mesh creation
 * This is a good place to initialize your mesh object */
static void my_3d_mesh__begin_solid(
        void* cookie, const struct gmio_stl_mesh_creator_infos* infos)
{
    struct my_3d_mesh* my_mesh = (struct my_3d_mesh*)cookie;
    size_t tri_count = 0;
    if (infos->format == GMIO_STL_FORMAT_ASCII) /* Assume facet size ~200B */
        tri_count = infos->stla_stream_size / 200u;
    else /* Binary STL */
        tri_count = infos->stlb_triangle_count;
    my_mesh->triangle_array =
            (struct my_3d_triangle*)malloc(
                tri_count * sizeof(struct my_3d_triangle));
    my_mesh->triangle_array_count = 0;
    my_mesh->triangle_array_capacity = tri_count;
}

/* Callback invoked sequentially for each triangle in the STL mesh
 * Just do something with the "triangle" passed in  */
static void my_3d_mesh__copy_triangle(
        void* cookie,
        uint32_t triangle_id,
        const struct gmio_stl_triangle* triangle)
{
    struct my_3d_mesh* my_mesh = (struct my_3d_mesh*)cookie;

    if (triangle_id >= my_mesh->triangle_array_capacity) {
        /* Capacity of the triangle array is too small, this can happen only
         * when reading STL ascii data, where the count of triangles in not
         * precisely known.
         * To overcome this just grow capacity of the triangle array of 12.5% */
        size_t new_capacity = my_mesh->triangle_array_capacity;
        new_capacity += new_capacity >> 3;
        my_mesh->triangle_array =
                (struct my_3d_triangle*)realloc(
                    my_mesh->triangle_array,
                    new_capacity * sizeof(struct my_3d_triangle));
        my_mesh->triangle_array_capacity = new_capacity;
    }

    { /* Copy new triangle */
        struct my_3d_triangle* my_tri = &my_mesh->triangle_array[triangle_id];
        const struct gmio_stl_coords* tri_vertices = &triangle->v1;
        for (int i = 0; i < 3; ++i) {
            my_tri->vertex[i][0] = tri_vertices[i].x;
            my_tri->vertex[i][1] = tri_vertices[i].y;
            my_tri->vertex[i][2] = tri_vertices[i].z;
        }
    }
    my_mesh->triangle_array_count = triangle_id + 1;
}

int main(int argc, char** argv)
{
    int error = 0;
    if (argc > 1) {
        /* Path to the STL file */
        const char* filepath = argv[1];
        /* User-defined mesh object, to be constructed */
        struct my_3d_mesh my_mesh = {0};
        /* Holds callbacks functions */
        struct gmio_stl_mesh_creator mesh_creator = {0};

        /* Initialize the callback object */
        /* -- Cookie object passed to callbacks of gmio_stl_mesh_creator */
        mesh_creator.cookie = &my_mesh;
        /* -- Function called initially at the beginning of a STL solid(mesh) */
        mesh_creator.func_begin_solid = &my_stl_mesh__begin_solid;
        /* -- Function called  for each triangle in the STL mesh */
        mesh_creator.func_add_triangle = &my_stl_mesh__copy_triangle;

        /* Read, using default options(NULL) */
        error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
        if (error != GMIO_ERROR_OK)
            fprintf(stderr, "gmio error: 0x%X\n", error);

        free(my_mesh.triangle_array);
    }
    return error;
}
