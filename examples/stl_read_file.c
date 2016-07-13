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
    if (infos->format == GMIO_STL_FORMAT_ASCII) /* Assume facet size ~230B */
        tri_count = infos->stla_stream_size / 230;
    else /* Binary STL */
        tri_count = infos->stlb_triangle_count;
    my_mesh->triangle_array =
            (struct my_3d_triangle*)malloc(
                tri_count * sizeof(struct my_3d_triangle));
    my_mesh->triangle_array_count = 0;
    my_mesh->triangle_array_capacity = tri_count;
}

/* Callback invoked sequentially for each triangle in the STL mesh */
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
         * To overcome this:
         *    - instead of just using general gmio_stl_read(), call
         *      call gmio_stl_infos_probe(GMIO_STL_INFO_FLAG_FACET_COUNT) and then
         *      gmio_stla_read()
         *    - or just grow the capacity of your mesh, here the triangle array
         *      is grown by 12.5% */
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
        const struct gmio_vec3f* tri_vertices = &triangle->v1;
        int i;
        for (i = 0; i < 3; ++i) {
            my_tri->vertex[i].coords[0] = tri_vertices[i].x;
            my_tri->vertex[i].coords[1] = tri_vertices[i].y;
            my_tri->vertex[i].coords[2] = tri_vertices[i].z;
        }
    }
    my_mesh->triangle_array_count = triangle_id + 1;
}

int main(int argc, char** argv)
{
    int error = 0;
    if (argc > 1) {
        const char* filepath = argv[1];
        struct my_3d_mesh my_mesh = {0};
        struct gmio_stl_mesh_creator mesh_creator = {0};

        /* Initialize the callback object */
        mesh_creator.cookie = &my_mesh;
        mesh_creator.func_begin_solid = my_3d_mesh__begin_solid;
        mesh_creator.func_add_triangle = my_3d_mesh__copy_triangle;

        /* Read, using default options(NULL) */
        error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
        if (error != GMIO_ERROR_OK)
            fprintf(stderr, "gmio error: 0x%X\n", error);

        free(my_mesh.triangle_array);
    }
    return error;
}
