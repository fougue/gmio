#ifndef _Handle_MeshVS_DataSource_HeaderFile
#define _Handle_MeshVS_DataSource_HeaderFile

class MeshVS_DataSource;

class Handle_MeshVS_DataSource
{
public:
    Handle_MeshVS_DataSource() {}
    Handle_MeshVS_DataSource(const Handle_MeshVS_DataSource&) {}
    Handle_MeshVS_DataSource(const MeshVS_DataSource*) {}

    MeshVS_DataSource* operator->() const
    { return 0; }

    Handle_MeshVS_DataSource& operator=(const MeshVS_DataSource*)
    { return *this; }
};

#endif // _Handle_MeshVS_DataSource_HeaderFile
