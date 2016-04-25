#ifndef _TopLoc_Location_HeaderFile
#define _TopLoc_Location_HeaderFile

#include <gp_Trsf.hxx>

class TopLoc_Location 
{
public:
    TopLoc_Location() {}
    const gp_Trsf& Transformation() const { return m_trsf; }

private:
    gp_Trsf m_trsf;
};

#endif // _TopLoc_Location_HeaderFile
