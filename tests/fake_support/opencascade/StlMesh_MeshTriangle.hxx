#ifndef _StlMesh_MeshTriangle_HeaderFile
#define _StlMesh_MeshTriangle_HeaderFile

class StlMesh_MeshTriangle
{
public:
    void GetVertexAndOrientation(
            int& /*V1*/, int& /*V2*/, int& /*V3*/,
            double& /*Xn*/, double& /*Yn*/, double& /*Zn*/) const
    { }
};

#include "generic_handle.h"
typedef FakeOcc::GenericHandle<StlMesh_MeshTriangle> Handle_StlMesh_MeshTriangle;

#endif // _StlMesh_MeshTriangle_HeaderFile
