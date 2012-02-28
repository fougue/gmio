#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QScopedPointer>
#include <QtCore/QStringList>
#include <QtCore/QTime>

#include <QtCore/QtDebug>

#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <StlMesh_Mesh.hxx>

#include "streams/qt4_stream.h"
#include "libstl/stlb.h"

namespace internal {

class OccStlMeshBuilder : public foug::stl::bin::AbstractGeometryBuilder
{
public:
  void beginTriangles(foug::UInt32 count)
  {
    Q_UNUSED(count);
    m_stlMesh = new StlMesh_Mesh;
    m_stlMesh->AddDomain();
  }

  void nextTriangle(const foug::stl::Triangle& triangle, foug::UInt16 attributeByteCount)
  {
    Q_UNUSED(attributeByteCount);
    const int uId = m_stlMesh->AddOnlyNewVertex(triangle.v1.x, triangle.v1.y, triangle.v1.z);
    const int vId = m_stlMesh->AddOnlyNewVertex(triangle.v2.x, triangle.v2.y, triangle.v2.z);
    const int wId = m_stlMesh->AddOnlyNewVertex(triangle.v3.x, triangle.v3.y, triangle.v3.z);
    const foug::stl::Coords& n = triangle.normal;
    m_stlMesh->AddTriangle(uId, vId, wId, n.x, n.y, n.z);
  }

  Handle_StlMesh_Mesh stlMesh() const
  {
    return m_stlMesh;
  }

private:
  Handle_StlMesh_Mesh m_stlMesh;
};

} // namespace internal

int main(int argc, char** argv)
{
  QStringList filePaths;
  for (int i = 1; i < argc; ++i)
    filePaths.append(argv[i]);

  const int benchCount = 1;
  QTime time;
  time.start();

  qDebug() << sizeof(foug::stl::Triangle);

  for (int i = 0; i < benchCount; ++i) {
    foreach (const QString& filePath, filePaths) {
      Handle_StlMesh_Mesh stlMesh = RWStl::ReadBinary(OSD_Path(filePath.toAscii().constData()));
    }
  }

  qDebug() << QString("OCC read time : %1sec").arg(time.elapsed() / 1000.);


  time.restart();

  internal::OccStlMeshBuilder occStlMeshBuilder;
  foug::stl::bin::Io io;
  io.setAutoDeleteStream(false);
  for (int i = 0; i < benchCount; ++i) {
    foreach (const QString& filePath, filePaths) {
      QFile file(filePath);
      if (file.open(QIODevice::ReadOnly)) {
        foug::Qt4Stream stream(&file);
        io.setStream(&stream);
        io.read(&occStlMeshBuilder);
        Handle_StlMesh_Mesh stlMesh = occStlMeshBuilder.stlMesh();
      }
    }
  }

  qDebug() << QString("FougSTL read time : %1sec").arg(time.elapsed() / 1000.);

  return 0;
}
