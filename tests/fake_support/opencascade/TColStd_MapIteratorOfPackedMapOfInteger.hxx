#ifndef TColStd_MapIteratorOfPackedMapOfInteger_HeaderFile
#define TColStd_MapIteratorOfPackedMapOfInteger_HeaderFile

#include <Standard_TypeDef.hxx>
class TColStd_PackedMapOfInteger;

class TColStd_MapIteratorOfPackedMapOfInteger
{
public:
    TColStd_MapIteratorOfPackedMapOfInteger() {}
    TColStd_MapIteratorOfPackedMapOfInteger(const TColStd_PackedMapOfInteger&) {}
    void Initialize(const TColStd_PackedMapOfInteger&) {}
    void Reset() {}
    int Key() const { return 0; }
    void Next() {}
    Standard_Boolean More() const { return Standard_False; }
};

#endif
