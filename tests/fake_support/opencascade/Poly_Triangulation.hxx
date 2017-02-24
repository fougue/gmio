#ifndef _Poly_Triangulation_HeaderFile
#define _Poly_Triangulation_HeaderFile

#include <Standard_TypeDef.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TShort_HArray1OfShortReal.hxx>

class Poly_Triangulation
{
public:
    Poly_Triangulation(int nbNodes, int nbTriangles, Standard_Boolean /*UVNodes*/)
        : myNodes(1, nbNodes),
          myTriangles(1, nbTriangles)
    {}

    const TColgp_Array1OfPnt& Nodes() const { return myNodes; }
    TColgp_Array1OfPnt& ChangeNodes() { return myNodes; }
    const Poly_Array1OfTriangle& Triangles() const { return myTriangles; }
    Poly_Array1OfTriangle& ChangeTriangles() { return myTriangles; }
    Standard_Integer NbTriangles() const { return 0; }

    void SetNormals(const Handle_TShort_HArray1OfShortReal&) {}
    const TShort_Array1OfShortReal& Normals() const { return myNormals->Array1(); }
    TShort_Array1OfShortReal& ChangeNormals() { return myNormals->ChangeArray1(); }
    Standard_Boolean HasNormals() const { return !myNormals.IsNull(); }

private: 
    TColgp_Array1OfPnt myNodes;
    Poly_Array1OfTriangle myTriangles;
    Handle_TShort_HArray1OfShortReal myNormals;
};

#include <Standard_Handle.hxx>
typedef opencascade::handle<Poly_Triangulation> Handle_Poly_Triangulation;

#endif // _Poly_Triangulation_HeaderFile
