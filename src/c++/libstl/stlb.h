#ifndef FOUG_CPP_LIBSTL_STLB_H
#define FOUG_CPP_LIBSTL_STLB_H

#include "abstract_geometry.h"
#include "stl_global.h"
#include "../abstract_stream.h"
#include "../io_base.h"

namespace foug {
namespace stlb {

enum { HeaderSize = 80 };
typedef uint8_t Header[HeaderSize];

class FOUG_STL_EXPORT AbstractGeometryBuilder
{
public:
  virtual void processHeader(const Header& data);
  virtual void beginTriangles(uint32_t count);
  virtual void processNextTriangle(const stl::Triangle& triangle, uint16_t attributeByteCount) = 0;
  virtual void endTriangles();
};

class FOUG_STL_EXPORT AbstractGeometryExtraData
{
public:
  virtual void getHeader(Header& data) const = 0;
  virtual uint16_t attributeByteCount(uint32_t triangleIndex) const = 0;
};

class FOUG_STL_EXPORT Io : public IoBase
{
public:
  Io(AbstractStream* stream = 0);

  bool read(AbstractGeometryBuilder* builder);
  bool write(const stl::AbstractGeometry& geom, const AbstractGeometryExtraData* extraData = 0);
};

} // namespace stlb
} // namespace foug

#endif // FOUG_CPP_LIBSTL_STLB_H
