#ifndef _Handle_StlMesh_MeshTriangle_HeaderFile
#define _Handle_StlMesh_MeshTriangle_HeaderFile

#include <cstddef>

class StlMesh_MeshTriangle;

class Handle_StlMesh_MeshTriangle
{
public:
    StlMesh_MeshTriangle* operator->() const
    { return NULL; }
};

#endif // _Handle_StlMesh_MeshTriangle_HeaderFile
