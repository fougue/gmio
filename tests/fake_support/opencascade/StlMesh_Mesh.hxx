#ifndef _StlMesh_Mesh_HeaderFile
#define _StlMesh_Mesh_HeaderFile

#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>

class StlMesh_Mesh
{
public:
    StlMesh_Mesh()
    { }

    virtual void AddDomain() { }

    virtual int AddTriangle(
            const int /*V1*/, const int /*V2*/, const int /*V3*/,
            const double /*Xn*/, const double /*Yn*/, const double /*Zn*/)
    { return -1; }

    virtual int AddVertex(
            const double /*X*/, const double /*Y*/, const double /*Z*/)
    { return -1; }

    virtual int AddOnlyNewVertex(
            const double /*X*/, const double /*Y*/, const double /*Z*/)
    { return -1; }

    virtual int NbTriangles(const int /*DomainIndex*/) const { return 0; }

    virtual int NbDomains() const { return 0; }

    virtual const StlMesh_SequenceOfMeshTriangle& Triangles(
            const int /*DomainIndex = 1*/) const
    {
        static StlMesh_SequenceOfMeshTriangle triSeq;
        return triSeq;
    }

    virtual const TColgp_SequenceOfXYZ& Vertices(
            const int /*DomainIndex = 1*/) const
    {
        static TColgp_SequenceOfXYZ vertSeq;
        return vertSeq;
    }
};

#include <Standard_Handle.hxx>
typedef opencascade::handle<StlMesh_Mesh> Handle_StlMesh_Mesh;

#endif // _StlMesh_Mesh_HeaderFile
