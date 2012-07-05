#ifndef FOUG_STLB_H
#define FOUG_STLB_H

#include "abstract_geometry.h"
#include "stl_global.h"
#include "../abstract_stream.h"
#include "../io_base.h"

namespace foug {
namespace stlb {

enum { HeaderSize = 80 };
typedef UInt8 Header[HeaderSize];

class FOUG_STL_EXPORT AbstractGeometryBuilder
{
public:
  virtual void processHeader(const Header& data);
  virtual void beginTriangles(UInt32 count);
  virtual void processNextTriangle(const stl::Triangle& triangle, UInt16 attributeByteCount) = 0;
  virtual void endTriangles();
};

class FOUG_STL_EXPORT AbstractGeometryExtraData
{
public:
  virtual void getHeader(Header& data) const = 0;
  virtual UInt16 attributeByteCount(UInt32 triangleIndex) const = 0;
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

#endif // FOUG_STLB_H
