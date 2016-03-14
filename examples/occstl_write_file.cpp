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
#include <gmio_support/stl_occ.h>
#include <StlMesh_Mesh.hxx>
#include <iostream>

int main(int argc, char** argv)
{
    int error = 0;
    if (argc > 1) {
        const char* filepath = argv[1];
        Handle_StlMesh_Mesh occmesh = new StlMesh_Mesh;
        // Initialize the OpenCascade StlMesh_Mesh here
        // ...

        const gmio_stl_occmesh_iterator occmesh_it(occmesh);
        const gmio_stl_mesh mesh = gmio_stl_occmesh(occmesh_it);

        // Write binary STL(little-endian), using default options(NULL)
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_LE, filepath, &mesh, NULL);
        if (error != GMIO_ERROR_OK)
            std::cerr << "gmio error: 0x" << std::hex << error << std::endl;
    }
    return error;
}
