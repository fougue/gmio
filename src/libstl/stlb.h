#ifndef FOUG_STLB_H
#define FOUG_STLB_H

#include "stl_global.h"
#include "abstract_geometry.h"
#include "io_base.h"
#include <abstract_stream.h>

namespace foug {
namespace stl {
namespace bin {

typedef UInt8 Header[80];

class FOUG_STL_EXPORT AbstractGeometryBuilder
{
public:
  virtual void header(const Header& data);
  virtual void beginTriangles(UInt32 count);
  virtual void nextTriangle(const stl::Triangle& triangle, UInt16 attributeByteCount) = 0;
  virtual void endTriangles();
};

class FOUG_STL_EXPORT AbstractGeometryExtraData
{
public:
  virtual void getHeaderData(Header& data) const = 0;
  virtual UInt16 attributeByteCount(UInt32 triangleIndex) const = 0;
};

class FOUG_STL_EXPORT Io : public IoBase
{
public:
  Io(AbstractStream* stream = 0);

  enum ReadError
  {
    NoReadError,
    StreamReadError,
    UnexpectedSizeReadError
  };
  ReadError read(AbstractGeometryBuilder* builder);

  enum WriteError
  {
    NoWriteError
  };
  WriteError write(const AbstractGeometry& geom, const AbstractGeometryExtraData* extraData = 0);
};

} // namespace bin
} // namespace stl
} // namespace foug

#endif // FOUG_STLB_H
