#ifndef _Standard_TypeDef_HeaderFile
#define _Standard_TypeDef_HeaderFile

typedef int           Standard_Integer;
typedef double        Standard_Real;
typedef unsigned int  Standard_Boolean;
typedef float         Standard_ShortReal;
typedef char          Standard_Character;
typedef short         Standard_ExtCharacter;
typedef unsigned char Standard_Byte;
typedef void*         Standard_Address;

typedef const char*   Standard_CString;
typedef const short*  Standard_ExtString;

#ifndef Standard_False
#  define Standard_False (Standard_Boolean)0
#endif
#ifndef Standard_True
#  define Standard_True  (Standard_Boolean)1
#endif

#endif // _Standard_TypeDef_HeaderFile
