#ifndef FAKE_OCC_GENERIC_HANDLE
#define FAKE_OCC_GENERIC_HANDLE

#include <Standard_TypeDef.hxx>

namespace FakeOcc {

template<typename T>
class GenericHandle
{
public:
    GenericHandle() { }
    GenericHandle(const GenericHandle<T>&) { }
    GenericHandle(const T*) { }
    Standard_Boolean IsNull() const { return Standard_True; }
    T* operator->() const { return NULL; }
    GenericHandle<T>& operator=(const T*) { return *this; }
};

} // namespace FakeOcc

#endif // FAKE_OCC_GENERIC_HANDLE
