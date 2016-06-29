#ifndef _gp_XYZ_HeaderFile
#define _gp_XYZ_HeaderFile

#include <Standard_TypeDef.hxx>

class gp_XYZ 
{
public:
    gp_XYZ() {}
    gp_XYZ(Standard_Real /*x*/, Standard_Real /*y*/, Standard_Real /*z*/) {}
    Standard_Real X() const { return 0.; }
    Standard_Real Y() const { return 0.; }
    Standard_Real Z() const { return 0.; }
};

#endif // _gp_XYZ_HeaderFile
