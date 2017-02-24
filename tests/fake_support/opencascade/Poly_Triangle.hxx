#ifndef _Poly_Triangle_HeaderFile
#define _Poly_Triangle_HeaderFile

#include <Standard_TypeDef.hxx>

class Poly_Triangle 
{
public:
    Poly_Triangle() {}
    Poly_Triangle(int, int, int) {}
    void Get(int&, int&, int&) const {}
    void Set(int, int, int) {}
private:
    int myNodes[3];
};

#endif // _Poly_Triangle_HeaderFile
