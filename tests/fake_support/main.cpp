#include "../../src/gmio_stl/stl_io.h"
#include "../../src/gmio_support/stl_occ_mesh.h"
#include "../../src/gmio_support/stream_qt.h"
#include "../../src/gmio_support/stream_cpp.h"

#include <QtCore/QFile>
#include <StlMesh_Mesh.hxx>
#include <fstream>

int main()
{
    // OpenCascade
    Handle_StlMesh_Mesh stlMesh;
    gmio_stl_occmesh_creator(stlMesh);

    // Qt
    QFile file;
    gmio_stream_qiodevice(&file);

    // C++
    std::ifstream is("test.txt");
    gmio_istream_cpp(&is);
    std::ofstream os("test.txt");
    gmio_ostream_cpp(&os);

    return 0;
}
