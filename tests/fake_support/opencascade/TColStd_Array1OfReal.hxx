#ifndef _TColStd_Array1OfReal_HeaderFile
#define _TColStd_Array1OfReal_HeaderFile

class TColStd_Array1OfReal
{
public:
    TColStd_Array1OfReal(const int, const int) {}

    int Length() const { return 0; }
    int Lower() const  { return 1; }
    int Upper() const  { return 1; }

    void SetValue (const int, const double&) {}

    const double& Value(const int) const
    {
        static const double v = 0;
        return v;
    }

    const double& operator()(const int Index) const
    { return Value(Index); }

    double& ChangeValue(const int)
    {
        static double v = 0;
        return v;
    }

    double& operator()(const int Index)
    { return ChangeValue(Index); }
};

#endif // _TColStd_Array1OfReal_HeaderFile
