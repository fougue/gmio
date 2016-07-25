#ifndef _TColgp_SequenceOfXYZ_HeaderFile
#define _TColgp_SequenceOfXYZ_HeaderFile

#include <gp_XYZ.hxx>

class TColgp_SequenceOfXYZ
{
public:
    struct const_iterator
    {
        const gp_XYZ& operator*() const
        {
            static const gp_XYZ coords;
            return coords;
        }

        const_iterator& operator++() { return *this; }

        bool operator!=(const const_iterator& /*other*/) const
        { return true; }
    };

    int Size() const { return 0; }
    int Length() const { return this->Size(); }

    const gp_XYZ& Value(const int /*Index*/) const
    {
        static gp_XYZ val;
        return val;
    }

    const_iterator cbegin() const { return cend(); }
    const_iterator cend() const { return const_iterator(); }
};

#endif // _TColgp_SequenceOfXYZ_HeaderFile
