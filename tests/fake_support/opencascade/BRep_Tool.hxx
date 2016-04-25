#ifndef _BRep_Tool_HeaderFile
#define _BRep_Tool_HeaderFile

#include <Standard_TypeDef.hxx>
#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Face.hxx>

class BRep_Tool 
{
public:
  static const Handle_Poly_Triangulation& Triangulation(
          const TopoDS_Face& /*F*/, TopLoc_Location& /*L*/)
  {
      static Handle_Poly_Triangulation ply;
      return ply;
  }
};

#endif // _BRep_Tool_HeaderFile
