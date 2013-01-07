#ifndef FOUG_CPP_LIBSTL_ABSTRACT_GEOMETRY_H
#define FOUG_CPP_LIBSTL_ABSTRACT_GEOMETRY_H

#include "stl_global.h"

namespace foug {

template <typename NUMERIC>
struct Coords3d
{
  NUMERIC x;
  NUMERIC y;
  NUMERIC z;
};

template <typename COORDS>
struct Triangle
{
  COORDS normal;
  COORDS v1;
  COORDS v2;
  COORDS v3;
};

namespace stl {

typedef Coords3d<Real32> Coords;
typedef Triangle<Coords> Triangle;

class FOUG_STL_EXPORT AbstractGeometry
{
public:
  virtual UInt32 triangleCount() const = 0;
  virtual void getTriangle(UInt32 index, Triangle* triangle) const = 0;
};

} // namespace stl
} // namespace foug

#endif // FOUG_CPP_LIBSTL_ABSTRACT_GEOMETRY_H
