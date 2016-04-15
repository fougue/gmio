#ifndef _MeshVS_DataSource_HeaderFile
#define _MeshVS_DataSource_HeaderFile

#include "Standard_TypeDef.hxx"
#include "Handle_MeshVS_DataSource.hxx"
class TColStd_Array1OfReal;
class TColStd_PackedMapOfInteger;

typedef enum
{
  MeshVS_ET_NONE   = 0x00,
  MeshVS_ET_Node   = 0x01,
  MeshVS_ET_0D     = 0x02,
  MeshVS_ET_Link   = 0x04,
  MeshVS_ET_Face   = 0x08,
  MeshVS_ET_Volume = 0x10,

  MeshVS_ET_Element = MeshVS_ET_0D | MeshVS_ET_Link | MeshVS_ET_Face | MeshVS_ET_Volume,
  MeshVS_ET_All     = MeshVS_ET_Element | MeshVS_ET_Node

} MeshVS_EntityType;

class MeshVS_DataSource
{
public:
    virtual Standard_Boolean GetGeom(const int, const Standard_Boolean, TColStd_Array1OfReal&, int&, MeshVS_EntityType&) const = 0;
    virtual Standard_Boolean GetGeomType(const int, const Standard_Boolean, MeshVS_EntityType&) const = 0;
    virtual const TColStd_PackedMapOfInteger& GetAllNodes() const = 0;
    virtual const TColStd_PackedMapOfInteger& GetAllElements() const = 0;

    virtual Standard_Boolean GetNormal(const int, const int, double&, double&, double&) const
    { return 0; }

    virtual Standard_Boolean GetNodeNormal(const int, const int, double&, double&, double&)  const
    { return 0; }

    virtual void GetAllGroups (TColStd_PackedMapOfInteger&) const
    { }

    virtual Standard_Boolean GetGroup (const int, MeshVS_EntityType&, TColStd_PackedMapOfInteger&) const
    { return 0; }
};

#endif // _MeshVS_DataSource_HeaderFile
