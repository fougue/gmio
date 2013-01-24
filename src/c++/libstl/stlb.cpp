#include "stlb.h"
#include "../abstract_task_progress.h"

#include <algorithm>
#include <limits>

// Read tools

template<typename NUMERIC>
NUMERIC fromLittleEndian(const uint8_t* bytes)
{
  return 0;
}

template<>
uint16_t fromLittleEndian<uint16_t>(const uint8_t* bytes)
{
  // |BB|AA| -> 0xAABB
  return (bytes[1] << 8) | bytes[0];
}

template<>
uint32_t fromLittleEndian<uint32_t>(const uint8_t* bytes)
{
  // |DD|CC|BB|AA| -> 0xAABBCCDD
  return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

template<>
foug::Real32 fromLittleEndian<foug::Real32>(const uint8_t* bytes)
{
  union
  {
    uint32_t asInteger;
    foug::Real32 asFloat;
  } helper;

  helper.asInteger = fromLittleEndian<uint32_t>(bytes);
  return helper.asFloat;
}

// Write tools

template<typename NUMERIC>
void toLittleEndian(NUMERIC n, uint8_t* bytes)
{
  union {
    NUMERIC asNumeric;
    uint8_t asBytes[sizeof(NUMERIC)];
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
static const int stlTriangleDataSize = (4 * 3) * sizeof(foug::Real32) + sizeof(uint16_t);

void AbstractGeometryBuilder::processHeader(const Header& /*data*/)
{
}

void AbstractGeometryBuilder::beginTriangles(uint32_t /*count*/)
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

  if (this->stream() == 0)
    return false;

  AbstractStream* istream = this->stream();
  AbstractTaskProgress* progress = this->taskProgress();
  const uint32_t chunkSize = stlTriangleDataSize * 163;

  uint8_t buffer[8192];
  char* charBuffer = reinterpret_cast<char*>(buffer);

  // Read header
  Header headerData;
  if (istream->read(reinterpret_cast<char*>(&headerData), HeaderSize) != HeaderSize)
    return false;
  builder->processHeader(headerData);

  // Read facet count
  if (istream->read(charBuffer, sizeof(uint32_t)) != sizeof(uint32_t))
    return false;
  const uint32_t facetCount = ::fromLittleEndian<uint32_t>(buffer);
  builder->beginTriangles(facetCount);

  if (progress != 0) {
    progress->reset();
    progress->setRange(0., facetCount);
  }

  // Read triangles
  const uint64_t totalFacetSize = stlTriangleDataSize * facetCount;
  uint64_t amountReadSize = 0;
  stl::Triangle triangle;
  bool streamError = false;
  while (amountReadSize < totalFacetSize && !streamError) {
    const int64_t iReadSize = istream->read(charBuffer, chunkSize);
    if (iReadSize > 0 && (iReadSize % stlTriangleDataSize == 0)) {
      const uint32_t iFacetCount = iReadSize / stlTriangleDataSize;
      uint32_t bufferOffset = 0;
      for (uint32_t i = 0; i < iFacetCount; ++i) {
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
        const uint16_t attributeByteCount =
            ::fromLittleEndian<uint16_t>(buffer + 12*sizeof(Real32) + bufferOffset);

        // Add triangle
        builder->processNextTriangle(triangle, attributeByteCount);

        bufferOffset += stlTriangleDataSize;
      } // end for

      if (progress != 0) {
        if (progress->isTaskStopRequested()) {
          streamError = true;
          progress->taskStoppedEvent();
        }
        else {
          progress->setValue(amountReadSize / stlTriangleDataSize);
        }
      }

      amountReadSize += iReadSize;
    }
    else {
      streamError = true;
    }
  } // end while

  if (!streamError)
    builder->endTriangles();

  return !streamError;
}

bool Io::write(const stl::AbstractGeometry& geom, const AbstractGeometryExtraData* extraData)
{
  if (this->stream() == 0)
    return false;

  AbstractStream* ostream = this->stream();
  AbstractTaskProgress* progress = this->taskProgress();

  uint8_t buffer[128];

  // Write header
  Header headerData;
  if (extraData != 0)
    extraData->getHeader(headerData);
  else
    std::fill(headerData, headerData + HeaderSize, 0);

  if (ostream->write(reinterpret_cast<char*>(&headerData), HeaderSize) != HeaderSize)
    return false;

  // Write facet count
  const uint32_t facetCount = geom.triangleCount();
  ::toLittleEndian<uint32_t>(facetCount, buffer);
  if (ostream->write(reinterpret_cast<char*>(&buffer), sizeof(uint32_t)) != sizeof(uint32_t))
    return false;

  if (progress != 0) {
    progress->reset();
    progress->setRange(0., facetCount);
  }

  // Write triangles
  stl::Triangle triangle;
  for (uint32_t facet = 0; facet < facetCount; ++facet) {
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
    const uint16_t attrByteCount = extraData != 0 ? extraData->attributeByteCount(facet) : 0;
    ::toLittleEndian<uint16_t>(attrByteCount, buffer + 12*sizeof(Real32));

    // Write to stream
    if (ostream->write(reinterpret_cast<const char*>(buffer), stlTriangleDataSize)
        != stlTriangleDataSize)
      return false;

    if (progress != 0) {
      if (progress->isTaskStopRequested()) {
        progress->taskStoppedEvent();
        return false;
      }
      else {
        progress->setValue(facet + 1);
      }
    }
  } // end for

  return true;
}

} // namespace stlb
} // namespace foug
