#ifndef _TopoDS_Shape_HeaderFile
#define _TopoDS_Shape_HeaderFile

#include <Standard_TypeDef.hxx>
#include <TopoDS_TShape.hxx>

enum TopAbs_Orientation
{
    TopAbs_FORWARD,
    TopAbs_REVERSED,
    TopAbs_INTERNAL,
    TopAbs_EXTERNAL
};

enum TopAbs_ShapeEnum
{
    TopAbs_COMPOUND,
    TopAbs_COMPSOLID,
    TopAbs_SOLID,
    TopAbs_SHELL,
    TopAbs_FACE,
    TopAbs_WIRE,
    TopAbs_EDGE,
    TopAbs_VERTEX,
    TopAbs_SHAPE
};

class TopoDS_Shape 
{
public:
    TopoDS_Shape() : myOrient(TopAbs_FORWARD) {}
    Standard_Boolean IsNull() const { return Standard_True; }
    TopAbs_Orientation Orientation() const { return myOrient; }

    const Handle_TopoDS_TShape& TShape() const { return myTShape; }
    void TShape(const Handle_TopoDS_TShape& other) { myTShape = other; }

private:
    Handle_TopoDS_TShape myTShape;
    TopAbs_Orientation myOrient;
};

#endif // _TopoDS_Shape_HeaderFile
