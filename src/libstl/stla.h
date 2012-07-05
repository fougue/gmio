#ifndef FOUG_STLA_H
#define FOUG_STLA_H

#include "stl_global.h"
#include "abstract_geometry.h"
#include "../io_base.h"
#include <string>

namespace foug {
namespace stla {

class FOUG_STL_EXPORT AbstractGeometryBuilder
{
public:
  virtual void beginSolid(const std::string& name);
  virtual void nextTriangle(const stl::Triangle& triangle) = 0;
  virtual void endSolid(const std::string& name);
};

class FOUG_STL_EXPORT Io : public IoBase
{
public:
  Io(AbstractStream* stream = 0);

  bool read(AbstractGeometryBuilder* builder, Int64 streamSize = -1);
  bool write(const stl::AbstractGeometry& geom, const std::string& solidName);
};

} // namespace stla
} // namespace foug

#endif // FOUG_STLA_H
