#ifndef _Handle_StlMesh_Mesh_HeaderFile
#define _Handle_StlMesh_Mesh_HeaderFile

#include <cstddef>

class StlMesh_Mesh;

class Handle_StlMesh_Mesh
{
public:
    StlMesh_Mesh* operator->() const
    { return NULL; }

    Handle_StlMesh_Mesh& operator=(const StlMesh_Mesh*)
    { return *this; }
};

#endif // _Handle_StlMesh_Mesh_HeaderFile
