#ifndef _StlMesh_MeshTriangle_HeaderFile
#define _StlMesh_MeshTriangle_HeaderFile

#include <Handle_StlMesh_MeshTriangle.hxx>

class StlMesh_MeshTriangle
{
public:
    void GetVertexAndOrientation(
            int& /*V1*/, int& /*V2*/, int& /*V3*/,
            double& /*Xn*/, double& /*Yn*/, double& /*Zn*/) const
    { }
};

#endif // _StlMesh_MeshTriangle_HeaderFile
