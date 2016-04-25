#ifndef _gp_Trsf_HeaderFile
#define _gp_Trsf_HeaderFile

#include <Standard_TypeDef.hxx>

enum gp_TrsfForm
{
    gp_Identity,
    gp_Rotation,
    gp_Translation,
    gp_PntMirror,
    gp_Ax1Mirror,
    gp_Ax2Mirror,
    gp_Scale,
    gp_CompoundTrsf,
    gp_Other
};

class gp_Trsf 
{
public:
    gp_Trsf() {}
    Standard_Boolean IsNegative() const { return Standard_False; }
    gp_TrsfForm Form() const { return gp_Identity; }
};

#endif // _gp_Trsf_HeaderFile
