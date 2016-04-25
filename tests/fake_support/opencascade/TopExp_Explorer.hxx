#ifndef _TopExp_Explorer_HeaderFile
#define _TopExp_Explorer_HeaderFile

#include <Standard_TypeDef.hxx>
#include <TopoDS_Shape.hxx>

class TopExp_Explorer 
{
public:
    TopExp_Explorer() {}
    TopExp_Explorer(
            const TopoDS_Shape& /*S*/,
            const TopAbs_ShapeEnum /*ToFind*/,
            const TopAbs_ShapeEnum /*ToAvoid*/ = TopAbs_SHAPE)
    {}

    Standard_Boolean More() const { return Standard_False; }
    void Next() {}
    const TopoDS_Shape& Current() const { return myShape; }
    void ReInit() {}

private:
    TopoDS_Shape myShape;
};

#endif // _TopExp_Explorer_HeaderFile
