#include "abstract_stream.h"
#include "stla.h"

#include <limits>

namespace foug {
namespace stl {
namespace asc {

bool Io::read(AbstractGeometryBuilder* builder, Int64 streamSize)
{
  return false;
}

bool Io::write(const AbstractGeometry& geom, const std::string& solidName)
{
  return false;
}

void AbstractGeometryBuilder::beginSolid(const std::string& /*name*/)
{
}

void AbstractGeometryBuilder::endSolid(const std::string& /*name*/)
{
}

} // namespace asc
} // namespace stl
} // namespace foug
