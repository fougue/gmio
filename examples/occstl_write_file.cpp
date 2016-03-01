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
#include <iostream>

int main(int argc, char** argv)
{
    int error = 0;
    if (argc > 1) {
        // Path to the STL file
        const char* filepath = argv[1];
        // StlMesh_Mesh object to be constructed
        Handle_StlMesh_Mesh occmesh = new StlMesh_Mesh;

        // Initialize the OpenCascade StlMesh_Mesh here
        // ...

        // Will give fast access to all the triangles of the StlMesh_Mesh object
        const gmio_stl_occmesh_iterator occmesh_it(occmesh);
        // The gmio interface over OpenCascade's StlMesh_Mesh
        const gmio_stl_mesh mesh = gmio_stl_occmesh(occmesh_it);

        // Write binary STL little-endian, using default options(NULL)
        error = gmio_stl_write_file(
                    GMIO_STL_FORMAT_BINARY_LE, filepath, &mesh, NULL);
        if (error != GMIO_ERROR_OK)
            std::cerr << "gmio error: 0x" << std::hex << error << std::endl;
    }
    return error;
}
