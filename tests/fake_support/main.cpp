#include "../../src/gmio_stl/stl_io.h"
#include "../../src/gmio_support/stl_occ.h"

#include <QtCore/QFile>
#include "../../src/gmio_support/stream_qt.h"

#include <Handle_StlMesh_Mesh.hxx>

#include "../../src/gmio_support/stream_cpp.h"
#include <fstream>

int main()
{
    // OpenCascade
    Handle_StlMesh_Mesh stlMesh;
    gmio_stl_hnd_occmesh_creator(stlMesh);

    // Qt
    QFile file;
    gmio_stream_qiodevice(&file);

    // C++
    //std::ifstream s("test.txt");
    //gmio_stream_t stream = gmio_stream_cpp(&s);

    return 0;
}
