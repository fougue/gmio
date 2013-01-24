#include "stla.h"
#include "../abstract_stream.h"

#include <limits>

namespace foug {
namespace stla {

bool Io::read(AbstractGeometryBuilder* builder, int64_t streamSize)
{
  return false;
}

bool Io::write(const stl::AbstractGeometry& geom, const std::string& solidName)
{
  return false;
}

void AbstractGeometryBuilder::beginSolid(const std::string& /*name*/)
{
}

void AbstractGeometryBuilder::endSolid(const std::string& /*name*/)
{
}

} // namespace stla
} // namespace foug
