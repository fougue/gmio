#ifndef _TColgp_SequenceOfXYZ_HeaderFile
#define _TColgp_SequenceOfXYZ_HeaderFile

#include <gp_XYZ.hxx>

class TColgp_SequenceOfXYZ
{
public:
    const gp_XYZ& Value(const int /*Index*/) const
    {
        static gp_XYZ val;
        return val;
    }
};

#endif // _TColgp_SequenceOfXYZ_HeaderFile
