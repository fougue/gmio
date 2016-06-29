#ifndef _gp_Pnt_HeaderFile
#define _gp_Pnt_HeaderFile

#include <Standard_TypeDef.hxx>
#include <gp_XYZ.hxx>
#include <gp_Trsf.hxx>

class gp_Pnt 
{
public:
    gp_Pnt() {}
    gp_Pnt(const gp_XYZ& /*coords*/) {}
    const gp_XYZ& XYZ() const { return coord; }
    void Transform(const gp_Trsf&) {}

private: 
    gp_XYZ coord;
};

#endif // _gp_Pnt_HeaderFile
