#ifndef TColStd_ListOfInteger_HeaderFile
#define TColStd_ListOfInteger_HeaderFile

#include "Standard_TypeDef.hxx"

class TColStd_ListOfInteger
{
public:
    TColStd_ListOfInteger() {}
    Standard_Boolean IsEmpty() const { return Standard_True; }
    int First() const { return -1; }
};


#endif
