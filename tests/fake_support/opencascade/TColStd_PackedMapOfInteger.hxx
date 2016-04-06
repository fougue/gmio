#ifndef TColStd_PackedMapOfInteger_HeaderFile
#define TColStd_PackedMapOfInteger_HeaderFile

#include "Standard_TypeDef.hxx"

class TColStd_PackedMapOfInteger
{
public:
    TColStd_PackedMapOfInteger(const int /*NbBuckets*/ = 1) {}
    TColStd_PackedMapOfInteger(const TColStd_PackedMapOfInteger&) {}
    ~TColStd_PackedMapOfInteger() {}

    TColStd_PackedMapOfInteger& operator=(const TColStd_PackedMapOfInteger&)
    { return *this; }

    void ReSize(const int) {}
    void Clear() {}

    Standard_Boolean Add(const int) { return Standard_False; }
    Standard_Boolean Contains(const int) const { return Standard_False; }
    Standard_Boolean Remove(const int) { return Standard_False; }

    int NbBuckets() const { return 0; }
    int Extent() const { return 0; }
    int IsEmpty() const { return 1; }
};

#endif
