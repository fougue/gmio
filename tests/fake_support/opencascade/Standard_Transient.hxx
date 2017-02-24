#pragma once

#include <Standard_Handle.hxx>
#include <Standard_TypeDef.hxx>
class Standard_Type;

class Standard_Transient
{
public:
  Standard_Transient() : count(0) {}
  Standard_Transient (const Standard_Transient&) : count(0) {}
  Standard_Transient& operator= (const Standard_Transient&) { return *this; }
  virtual ~Standard_Transient() {}
  virtual void Delete() const {}

  typedef void base_type;

  static const char* get_type_name () { return "Standard_Transient"; }

  static const opencascade::handle<Standard_Type>& get_type_descriptor()
  { static opencascade::handle<Standard_Type> d; return d; }

  virtual const opencascade::handle<Standard_Type>& DynamicType() const
  { static opencascade::handle<Standard_Type> dt; return dt; }

  Standard_Boolean IsInstance(const opencascade::handle<Standard_Type>&) const
  { return Standard_False; }

  Standard_Boolean IsInstance(const Standard_CString) const
  { return Standard_False; }

  Standard_Boolean IsKind(const opencascade::handle<Standard_Type>&) const
  { return Standard_False; }

  Standard_Boolean IsKind(const Standard_CString) const
  { return Standard_False; }

  Standard_Transient* This() const
  { return const_cast<Standard_Transient*> (this); }

  Standard_Integer GetRefCount() const { return count; }
  void IncrementRefCounter() const { ++count; }
  Standard_Integer DecrementRefCounter() const { return --count; }

private:
  mutable volatile Standard_Integer count;
};

//! Definition of Handle_Standard_Transient as typedef for compatibility
typedef opencascade::handle<Standard_Transient> Handle_Standard_Transient;
