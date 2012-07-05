#include "stlb.h"
#include "../abstract_task_progress.h"

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
namespace stlb {

static const int stlFacetSize = 50;
static const int stlMinFileSize = 284;
static const int stlTriangleDataSize = (4 * 3) * sizeof(foug::Real32) + sizeof(foug::UInt16);

void AbstractGeometryBuilder::processHeader(const Header& /*data*/)
{
}

void AbstractGeometryBuilder::beginTriangles(UInt32 /*count*/)
{
}

void AbstractGeometryBuilder::endTriangles()
{
}

Io::Io(AbstractStream *stream)
  : IoBase(stream)
{
}

bool Io::read(AbstractGeometryBuilder* builder)
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
  const UInt32 chunkSize = stlTriangleDataSize * 163;

  UInt8 buffer[8192];
  char* charBuffer = reinterpret_cast<char*>(buffer);

  // Read header
  Header headerData;
  if (istream->read(reinterpret_cast<char*>(&headerData), HeaderSize) != HeaderSize)
    return false;
  builder->processHeader(headerData);

  // Read facet count
  if (istream->read(charBuffer, sizeof(UInt32)) != sizeof(UInt32))
    return false;
  const UInt32 facetCount = ::fromLittleEndian<UInt32>(buffer);
  builder->beginTriangles(facetCount);

  if (progress != 0) {
    progress->reset();
    progress->setRange(0., facetCount);
  }

  // Read triangles
  const UInt64 totalFacetSize = stlTriangleDataSize * facetCount;
  UInt64 amountReadSize = 0;
  stl::Triangle triangle;
  bool streamError = false;
  while (amountReadSize < totalFacetSize && !streamError) {
    const Int64 iReadSize = istream->read(charBuffer, chunkSize);
    if (iReadSize > 0 && (iReadSize % stlTriangleDataSize == 0)) {
      const UInt32 iFacetCount = iReadSize / stlTriangleDataSize;
      UInt32 bufferOffset = 0;
      for (UInt32 i = 0; i < iFacetCount; ++i) {
        // Read normal
        triangle.normal.x = ::fromLittleEndian<Real32>(buffer + bufferOffset);
        triangle.normal.y = ::fromLittleEndian<Real32>(buffer + 1*sizeof(Real32) + bufferOffset);
        triangle.normal.z = ::fromLittleEndian<Real32>(buffer + 2*sizeof(Real32) + bufferOffset);

        // Read vertex1
        triangle.v1.x = ::fromLittleEndian<Real32>(buffer + 3*sizeof(Real32) + bufferOffset);
        triangle.v1.y = ::fromLittleEndian<Real32>(buffer + 4*sizeof(Real32) + bufferOffset);
        triangle.v1.z = ::fromLittleEndian<Real32>(buffer + 5*sizeof(Real32) + bufferOffset);

        // Read vertex2
        triangle.v2.x = ::fromLittleEndian<Real32>(buffer + 6*sizeof(Real32) + bufferOffset);
        triangle.v2.y = ::fromLittleEndian<Real32>(buffer + 7*sizeof(Real32) + bufferOffset);
        triangle.v2.z = ::fromLittleEndian<Real32>(buffer + 8*sizeof(Real32) + bufferOffset);

        // Read vertex3
        triangle.v3.x = ::fromLittleEndian<Real32>(buffer + 9*sizeof(Real32) + bufferOffset);
        triangle.v3.y = ::fromLittleEndian<Real32>(buffer + 10*sizeof(Real32) + bufferOffset);
        triangle.v3.z = ::fromLittleEndian<Real32>(buffer + 11*sizeof(Real32) + bufferOffset);

        // Attribute byte count
        const UInt16 attributeByteCount =
            ::fromLittleEndian<UInt16>(buffer + 12*sizeof(Real32) + bufferOffset);

        // Add triangle
        builder->processNextTriangle(triangle, attributeByteCount);

        bufferOffset += stlTriangleDataSize;
      }

      if (progress != 0)
        progress->setValue(amountReadSize / stlTriangleDataSize);

      amountReadSize += iReadSize;
    }
    else {
      streamError = true;
    }
  }

  builder->endTriangles();

  return !streamError;
}

bool Io::write(const stl::AbstractGeometry& geom, const AbstractGeometryExtraData* extraData)
{
  AbstractStream* ostream = this->stream();
  AbstractTaskProgress* progress = this->taskProgress();

  UInt8 buffer[128];

  // Write header
  Header headerData;
  if (extraData != 0)
    extraData->getHeader(headerData);
  else
    std::fill(headerData, headerData + HeaderSize, 0);

  if (ostream->write(reinterpret_cast<char*>(&headerData), HeaderSize) != HeaderSize)
    return false;

  // Write facet count
  const UInt32 facetCount = geom.triangleCount();
  ::toLittleEndian<UInt32>(facetCount, buffer);
  if (ostream->write(reinterpret_cast<char*>(&buffer), sizeof(UInt32)) != sizeof(UInt32))
    return false;

  if (progress != 0) {
    progress->reset();
    progress->setRange(0., facetCount);
  }

  // Write triangles
  stl::Triangle triangle;
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
    if (ostream->write(reinterpret_cast<const char*>(buffer), stlTriangleDataSize)
        != stlTriangleDataSize)
      return false;

    if (progress != 0)
      progress->setValue(facet + 1);
  }

  return true;
}

} // namespace stlb
} // namespace foug
