#ifndef _BRep_TFace_HeaderFile
#define _BRep_TFace_HeaderFile

#include <Poly_Triangulation.hxx>
#include <TopoDS_TShape.hxx>

class BRep_TFace : public TopoDS_TShape
{
public:
    BRep_TFace() {}

    const Handle_Poly_Triangulation& Triangulation() const
    { return myTriangulation; }

    void Triangulation (const Handle_Poly_Triangulation& other)
    { myTriangulation = other; }

private:
    Handle_Poly_Triangulation myTriangulation;
};

#include <Standard_Handle.hxx>
typedef opencascade::handle<BRep_TFace> Handle_BRep_TFace;

#endif // _BRep_TFace_HeaderFile
