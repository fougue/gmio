#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QScopedPointer>
#include <QtCore/QStringList>

#include "abstract_task_progress.h"
#include "libstl/abstract_geometry.h"
#include "libstl/stlb.h"
#include "streams/qt4_stream.h"

#include <fstream>
#include "streams/std_io_stream.h"

#include <cstring>
#include <QtCore/QtDebug>
#include <QtCore/QTime>
#include <QtCore/QVector>

#define USE_QT4STREAM
//#define USE_STDSTREAM

namespace internal {

class ConsoleTaskProgress : public foug::AbstractTaskProgress
{
public:
  void handleProgressUpdate()
  {
    qDebug() << this->progress();
  }
};

class DummyGeometryBuilder : public foug::stl::bin::AbstractGeometryBuilder
{
public:
  void processHeader(const foug::stl::bin::Header& data)
  {
    qDebug() << "HEADER : \n" << QString::fromAscii(reinterpret_cast<const char*>(data), 79);
  }

  void beginTriangles(foug::UInt32 count)
  {
    m_triangleCounter = 0;
    qDebug() << "Triangle count" << count;
  }

  void processNextTriangle(const foug::stl::Triangle& triangle, foug::UInt16 attributeByteCount)
  {
    //    qDebug() << "TRIANGLE " << m_triangleCounter++;
    //    qDebug() << "   normal" << triangle.normal.x << triangle.normal.y << triangle.normal.z;
    //    qDebug() << "   v1" << triangle.v1.x << triangle.v1.y << triangle.v1.z;
    //    qDebug() << "   v2" << triangle.v2.x << triangle.v2.y << triangle.v2.z;
    //    qDebug() << "   v3" << triangle.v3.x << triangle.v3.y << triangle.v3.z;
    //    qDebug() << "   attr" << attributeByteCount;
  }

private:
  foug::UInt32 m_triangleCounter;
};

class BasicGeometryHelper :
    public foug::stl::AbstractGeometry,
    public foug::stl::bin::AbstractGeometryBuilder,
    public foug::stl::bin::AbstractGeometryExtraData
{
public:
  struct TriangleData
  {
    foug::stl::Triangle triangle;
    foug::UInt16 attributeByteCount;
  };

  QVector<TriangleData> triangles() const
  {
    return m_triangles;
  }

  // -- foug::stl::bin::AbstractGeometryBuilder

  void processHeader(const foug::stlb::Header& data)
  {
    std::memcpy(m_header, data, 80);
  }

  void beginTriangles(foug::UInt32 count)
  {
    m_triangles.reserve(count);
    m_triangles.clear();
    m_triangles.resize(0);
  }

  void processNextTriangle(const foug::stl::Triangle& triangle, foug::UInt16 attributeByteCount)
  {
    TriangleData data;
    data.triangle = triangle;
    data.attributeByteCount = attributeByteCount;
    m_triangles.append(data);
  }

  // -- foug::stl::bin::AbstractGeometryExtraData

  void getHeaderData(foug::stl::bin::Header& data) const
  {
    std::memcpy(data, m_header, 80);
  }

  foug::UInt16 attributeByteCount(foug::UInt32 triangleIndex) const
  {
    return m_triangles.at(triangleIndex).attributeByteCount;
  }

  // -- foug::stl::AbstractGeometry

  foug::UInt32 triangleCount() const
  {
    return m_triangles.size();
  }

  void getTriangle(foug::UInt32 index, foug::stl::Triangle* triangle) const
  {
    *triangle = m_triangles.at(index).triangle;
  }

private:
  QVector<TriangleData> m_triangles;
  foug::stl::bin::Header m_header;
};

} // namespace internal

int main(int argc, char** argv)
{
  using namespace foug::stl;

  QString inputFilePath;
  QString outputFilePath;
  int iarg = 1;
  while (iarg < argc) {
    if (!std::strcmp(argv[iarg], "-i"))
      inputFilePath = argv[++iarg];
    else if (!std::strcmp(argv[iarg], "-o"))
      outputFilePath = argv[++iarg];
    ++iarg;
  }

  if (inputFilePath.isEmpty()) {
    qCritical() << "No input files";
    return -1;
  }

  bin::Io io;
  //  internal::ConsoleTaskProgress taskProgress;
  //  io.setTaskProgress(&taskProgress);
  //  io.setAutoDeleteTaskProgress(false);

#ifdef USE_STDSTREAM
  std::ifstream inputFile(inputFilePath.toStdString().c_str(), std::ios::binary);
  if (!inputFile.is_open()) {
    qCritical() << "Failed to open input file";
    return -2;
  }
  io.setStream(new foug::StdInputStream<std::ifstream>(&inputFile));
#elif defined(USE_QT4STREAM)
  QFile inputFile(inputFilePath);
  if (!inputFile.open(QIODevice::ReadOnly)) {
    qCritical() << "Failed to open input file :" << inputFile.errorString();
    return -2;
  }
  io.setStream(new foug::Qt4Stream(&inputFile));
#endif

  { // BENCH READ
    QTime time;
    time.start();

    internal::DummyGeometryBuilder geomBuilder;
    const int benchCount = 100;
    for (int i = 0; i < benchCount; ++i) {
      io.stream()->seek(0);
      io.read(&geomBuilder);
    }

    qDebug() << QString("Total read time : %1sec").arg(time.elapsed() / 1000.);
    qDebug() << QString("Test read time : %1sec")
                .arg(time.elapsed() / 1000. / static_cast<double>(benchCount));
  }

  if (outputFilePath.isEmpty())
    return 0;

#ifdef USE_STDSTREAM
  std::ofstream outputFile(outputFilePath.toStdString().c_str(), std::ios::out | std::ios::binary);
  //std::ofstream outputFile(outputFilePath.toStdString().c_str(), std::ios::binary);
  if (!outputFile.is_open()) {
    qCritical() << "Failed to open output file";
    return -3;
  }
#elif defined(USE_QT4STREAM)
  QFile outputFile(outputFilePath);
  if (!outputFile.open(QIODevice::WriteOnly))  {
    qCritical() << "Failed to open output file :" << outputFile.errorString();
    return -3;
  }
#endif

  internal::BasicGeometryHelper geomHelper;
  { // READ GEOMETRY
    io.stream()->seek(0);
    io.read(&geomHelper);
  }

  { // BENCH WRITE
#ifdef USE_STDSTREAM
    io.setStream(new foug::StdOutputStream<std::ofstream>(&outputFile));
#elif defined(USE_QT4STREAM)
    io.setStream(new foug::Qt4Stream(&outputFile));
#endif

    QTime time;
    time.start();

    const int benchCount = 100;
    for (int i = 0; i < benchCount; ++i) {
      io.stream()->seek(0);
      io.write(geomHelper, &geomHelper);
    }

    qDebug() << QString("Total write time : %1sec").arg(time.elapsed() / 1000.);
    qDebug() << QString("Test write time : %1sec")
                .arg(time.elapsed() / 1000. / static_cast<double>(benchCount));
  }

  return 0;
}
