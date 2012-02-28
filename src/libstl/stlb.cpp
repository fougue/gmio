#include "abstract_stream.h"
#include "stlb.h"
#include "abstract_task_progress.h"

#include <algorithm>
#include <limits>

// Read tools

template<typename NUMERIC>
NUMERIC fromLittleEndian(const foug::UInt8* bytes)
{
  return 0;
}

template<>
foug::UInt16 fromLittleEndian<foug::UInt16>(const foug::UInt8* bytes)
{
  // |BB|AA| -> 0xAABB
  return (bytes[1] << 8) | bytes[0];
}

template<>
foug::UInt32 fromLittleEndian<foug::UInt32>(const foug::UInt8* bytes)
{
  // |DD|CC|BB|AA| -> 0xAABBCCDD
  return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

template<>
foug::Real32 fromLittleEndian<foug::Real32>(const foug::UInt8* bytes)
{
  union
  {
    foug::UInt32 asInteger;
    foug::Real32 asFloat;
  } helper;

  helper.asInteger = fromLittleEndian<foug::UInt32>(bytes);
  return helper.asFloat;
}

// Write tools

template<typename NUMERIC>
void toLittleEndian(NUMERIC n, foug::UInt8* bytes)
{
  union {
    NUMERIC asNumeric;
    foug::UInt8 asBytes[sizeof(NUMERIC)];
  } helper;
  helper.asNumeric = n;
  //  std::copy(helper.asBytes, helper.asBytes + sizeof(NUMERIC), bytes);
  for (unsigned i = 0; i < sizeof(NUMERIC); ++i)
    bytes[i] = helper.asBytes[i];
}

namespace foug {
namespace stl {
namespace bin {

static const int stlHeaderSize = 80;
static const int stlFacetSize = 50;
static const int stlMinFileSize = 284;
static const int stlTriangleDataSize = (4 * 3) * sizeof(foug::Real32) + sizeof(foug::UInt16);

void AbstractGeometryBuilder::header(const Header& /*data*/)
{
}

void AbstractGeometryBuilder::beginTriangles(UInt32 /*count*/)
{
}

void AbstractGeometryBuilder::endTriangles()
{
}

Io::ReadError streamRead(AbstractStream* stream, char* buffer, Int64 size)
{
  const Int64 result = stream->read(buffer, size);
  if (result == -1)
    return Io::StreamReadError;
  else if (result != size)
    return Io::UnexpectedSizeReadError;
  return Io::NoReadError;
}

Io::ReadError getReadError(Int64 readResult, Int64 expectedSize)
{
  if (readResult == -1)
    return Io::StreamReadError;
  else if (readResult != expectedSize)
    return Io::UnexpectedSizeReadError;
  else
    return Io::NoReadError;
}

Io::Io(AbstractStream *stream)
  : IoBase(stream)
{
}

Io::ReadError Io::read(AbstractGeometryBuilder* builder)
{
  //  // Check file size
  //  const qint64 fileSize = stream->size();
  //  if (((fileSize - stlHeaderSize - stlFacetCountSize) % stlFacetSize != 0)
  //      || fileSize < stlMinFileSize) {
  //    cpp::checkedAssign(err, WrongFileSizeBinaryStlLoadError);
  //    return false;
  //  }

  //  const int facetCount = (fileSize - stlHeaderSize - stlFacetCountSize) / stlFacetSize;

  AbstractStream* istream = this->stream();
  AbstractTaskProgress* progress = this->taskProgress();
  //  Io::ReadError readErr = Io::NoReadError;

  UInt8 buffer[8192];
  char* charBuffer = reinterpret_cast<char*>(buffer);

  // Read header
  Header headerData;
  istream->read(reinterpret_cast<char*>(&headerData), 80);
  //  if ((readErr = streamRead(istream, reinterpret_cast<char*>(&headerData), 80)) != NoReadError)
  //    return readErr;
  builder->header(headerData);

  // Read facet count
  istream->read(charBuffer, sizeof(UInt32));
  //  if ((readErr = streamRead(istream, charBuffer, sizeof(UInt32))) != NoReadError)
  //    return readErr;
  const UInt32 facetCount = ::fromLittleEndian<UInt32>(buffer);
  builder->beginTriangles(facetCount);

  if (progress != 0) {
    progress->reset();
    progress->setRange(0., facetCount);
  }

  // Read triangles
  Triangle triangle;
  for (UInt32 facet = 0; facet < facetCount; ++facet) {
    istream->read(charBuffer, stlTriangleDataSize);
    //    if ((readErr = streamRead(istream, charBuffer, stlTriangleDataSize)) != NoReadError)
    //      return readErr;

    // Read normal
    triangle.normal.x = ::fromLittleEndian<Real32>(buffer);
    triangle.normal.y = ::fromLittleEndian<Real32>(buffer + 1*sizeof(Real32));
    triangle.normal.z = ::fromLittleEndian<Real32>(buffer + 2*sizeof(Real32));

    // Read vertex1
    triangle.v1.x = ::fromLittleEndian<Real32>(buffer + 3*sizeof(Real32));
    triangle.v1.y = ::fromLittleEndian<Real32>(buffer + 4*sizeof(Real32));
    triangle.v1.z = ::fromLittleEndian<Real32>(buffer + 5*sizeof(Real32));

    // Read vertex2
    triangle.v2.x = ::fromLittleEndian<Real32>(buffer + 6*sizeof(Real32));
    triangle.v2.y = ::fromLittleEndian<Real32>(buffer + 7*sizeof(Real32));
    triangle.v2.z = ::fromLittleEndian<Real32>(buffer + 8*sizeof(Real32));

    // Read vertex3
    triangle.v3.x = ::fromLittleEndian<Real32>(buffer + 9*sizeof(Real32));
    triangle.v3.y = ::fromLittleEndian<Real32>(buffer + 10*sizeof(Real32));
    triangle.v3.z = ::fromLittleEndian<Real32>(buffer + 11*sizeof(Real32));

    // Attribute byte count
    const UInt16 attributeByteCount = ::fromLittleEndian<UInt16>(buffer + 12*sizeof(Real32));

    // Add triangle
    builder->nextTriangle(triangle, attributeByteCount);

    if (progress != 0)
      progress->setValue(facet + 1);
  }

  builder->endTriangles();

  return NoReadError;
}

Io::WriteError Io::write(const AbstractGeometry& geom, const AbstractGeometryExtraData* extraData)
{
  AbstractStream* ostream = this->stream();
  AbstractTaskProgress* progress = this->taskProgress();

  UInt8 buffer[128];

  // Write header
  Header headerData;
  if (extraData != 0)
    extraData->getHeaderData(headerData);
  else
    std::fill(headerData, headerData + 80, 0);

  ostream->write(reinterpret_cast<char*>(&headerData), 80);

  // Write facet count
  const UInt32 facetCount = geom.triangleCount();
  ::toLittleEndian<UInt32>(facetCount, buffer);
  ostream->write(reinterpret_cast<char*>(&buffer), sizeof(UInt32));

  if (progress != 0) {
    progress->reset();
    progress->setRange(0., facetCount);
  }

  // Write triangles
  Triangle triangle;
  for (UInt32 facet = 0; facet < facetCount; ++facet) {
    geom.getTriangle(facet, &triangle);

    // Write normal
    ::toLittleEndian<Real32>(triangle.normal.x, buffer);
    ::toLittleEndian<Real32>(triangle.normal.y, buffer + 1*sizeof(Real32));
    ::toLittleEndian<Real32>(triangle.normal.z, buffer + 2*sizeof(Real32));

    // Write vertex1
    ::toLittleEndian<Real32>(triangle.v1.x, buffer + 3*sizeof(Real32));
    ::toLittleEndian<Real32>(triangle.v1.y, buffer + 4*sizeof(Real32));
    ::toLittleEndian<Real32>(triangle.v1.z, buffer + 5*sizeof(Real32));

    // Write vertex2
    ::toLittleEndian<Real32>(triangle.v2.x, buffer + 6*sizeof(Real32));
    ::toLittleEndian<Real32>(triangle.v2.y, buffer + 7*sizeof(Real32));
    ::toLittleEndian<Real32>(triangle.v2.z, buffer + 8*sizeof(Real32));

    // Write vertex3
    ::toLittleEndian<Real32>(triangle.v3.x, buffer + 9*sizeof(Real32));
    ::toLittleEndian<Real32>(triangle.v3.y, buffer + 10*sizeof(Real32));
    ::toLittleEndian<Real32>(triangle.v3.z, buffer + 11*sizeof(Real32));

    // Attribute byte count
    const UInt16 attrByteCount = extraData != 0 ? extraData->attributeByteCount(facet) : 0;
    ::toLittleEndian<UInt16>(attrByteCount, buffer + 12*sizeof(Real32));

    // Write to stream
    ostream->write(reinterpret_cast<const char*>(buffer), stlTriangleDataSize);

    if (progress != 0)
      progress->setValue(facet + 1);
  }

  return NoWriteError;
}

} // namespace bin
} // namespace stl
} // namespace foug
