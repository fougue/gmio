#ifndef _BRepBuilderAPI_VertexInspector_Header
#define _BRepBuilderAPI_VertexInspector_Header

#include "gp_XYZ.hxx"
#include "TColStd_ListOfInteger.hxx"

class BRepBuilderAPI_VertexInspector
{
public:
    BRepBuilderAPI_VertexInspector(double /*tol*/) {}

    void SetCurrent(const gp_XYZ& /*pnt*/) {}

    gp_XYZ Shift(const gp_XYZ& /*pnt*/, double tol) { return gp_XYZ(); }

    const TColStd_ListOfInteger& ResInd()
    {
        static const TColStd_ListOfInteger listInt;
        return listInt;
    }

    void ClearResList() {}

    void Add (const gp_XYZ& /*pnt*/) {}
};

#endif
