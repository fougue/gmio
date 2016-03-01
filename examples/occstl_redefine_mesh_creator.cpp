/* -----------------------------------------------------------------------------
 *
 * Example: redefine the callbacks of some gmio_stl_mesh_creator base object
 *
 * -------------------------------------------------------------------------- */

#include <gmio_core/error.h>
#include <gmio_stl/stl_io.h>
#include <gmio_support/stl_occ.h>
#include <iostream>

// Redefine func_begin_solid of some gmio_stl_mesh_creator object
void my_mesh_creator__begin_solid(
        void* cookie, const gmio_stl_mesh_creator_infos* infos)
{
    gmio_stl_mesh_creator* base_creator =
            reinterpret_cast<gmio_stl_mesh_creator*>(cookie);
    base_creator->func_begin_solid(base_creator->cookie, infos);
    // Do something more ...
}

// Redefine func_add_triangle of some gmio_stl_mesh_creator object
void my_mesh_creator__add_triangle(
        void* cookie, uint32_t tri_id, const gmio_stl_triangle* triangle)
{
    gmio_stl_mesh_creator* base_creator =
            reinterpret_cast<gmio_stl_mesh_creator*>(cookie);
    base_creator->func_add_triangle(base_creator->cookie, tri_id, triangle);
    // Do something more ...
}

int main(int argc, char** argv)
{
    int error = 0;
    if (argc > 1) {
        // Path to the STL file
        const char* filepath = argv[1];
        // StlMesh_Mesh object to be constructed
        Handle_StlMesh_Mesh mesh = new StlMesh_Mesh;
        // mesh_creator to be redefined
        gmio_stl_mesh_creator base_creator = gmio_stl_occmesh_creator(mesh);
        // Holds callbacks functions
        gmio_stl_mesh_creator creator = {};
        creator.cookie = &base_creator;
        creator.func_begin_solid = my_mesh_creator__begin_solid;
        creator.func_add_triangle = my_mesh_creator__add_triangle;

        // Read, using default options(NULL)
        error = gmio_stl_read_file(filepath, &creator, NULL);
        if (error != GMIO_ERROR_OK)
            std::cerr << "gmio error: 0x" << std::hex << error << std::endl;
    }
    return error;
}
