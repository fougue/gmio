#pragma once

#include <Standard_Transient.hxx>
#include <cstddef>

class Standard_Type : public Standard_Transient
{
public:
  Standard_CString SystemName() const { return mySystemName; }
  Standard_CString Name() const { return myName; }
  std::size_t Size() const { return mySize; }

private:
  Standard_CString mySystemName;
  Standard_CString myName;
  std::size_t mySize;
};

