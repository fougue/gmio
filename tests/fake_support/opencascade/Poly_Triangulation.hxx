#ifndef _Poly_Triangulation_HeaderFile
#define _Poly_Triangulation_HeaderFile

#include <Standard_TypeDef.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Array1OfTriangle.hxx>

class Poly_Triangulation
{
public:
  const TColgp_Array1OfPnt& Nodes() const { return myNodes; }
  const Poly_Array1OfTriangle& Triangles() const { return myTriangles; }
  Standard_Integer NbTriangles() const { return 0; }

private: 
  TColgp_Array1OfPnt myNodes;
  Poly_Array1OfTriangle myTriangles;
};

#include "generic_handle.h"
typedef FakeOcc::GenericHandle<Poly_Triangulation> Handle_Poly_Triangulation;

#endif // _Poly_Triangulation_HeaderFile
