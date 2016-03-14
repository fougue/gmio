/* -----------------------------------------------------------------------------
 *
 * Example: read a STL file into OpenCascade StlMesh_Mesh
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
#include <gmio_support/stl_occ.h>
#include <StlMesh_Mesh.hxx>
#include <iostream>

int main(int argc, char** argv)
{
    int error = 0;
    if (argc > 1) {
        const char* filepath = argv[1];
        Handle_StlMesh_Mesh mesh = new StlMesh_Mesh;
        gmio_stl_mesh_creator mesh_creator = gmio_stl_occmesh_creator(mesh);

        // Read, using default options(NULL)
        error = gmio_stl_read_file(filepath, &mesh_creator, NULL);
        if (error != GMIO_ERROR_OK)
            std::cerr << "gmio error: 0x" << std::hex << error << std::endl;
    }
    return error;
}
