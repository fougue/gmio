#ifndef GMIO_LIBSTL_TRIANGLE_H
#define GMIO_LIBSTL_TRIANGLE_H

#include "stl_global.h"

/*! \brief Cartesian coordinate entity in 3D space, specifically tailored for
 *         STL needs (single-float)
 */
struct gmio_stl_coords
{
  gmio_real32_t x;
  gmio_real32_t y;
  gmio_real32_t z;
};

typedef struct gmio_stl_coords gmio_stl_coords_t;

/*! \brief STL mesh triangle defined three geometric vertices and an
 *         orientation(normal)
 */
struct gmio_stl_triangle
{
  gmio_stl_coords_t normal;
  gmio_stl_coords_t v1;
  gmio_stl_coords_t v2;
  gmio_stl_coords_t v3;
  uint16_t attribute_byte_count; /*!< Useful only for STL binary format */
};

typedef struct gmio_stl_triangle gmio_stl_triangle_t;

/*! Compact size of a gmio_stl_coords_t object */
#define GMIO_STL_COORDS_RAWSIZE (3 * sizeof(gmio_real32_t))

/*! Compact size of a gmio_stl_triangle_t object for STL ascii format */
#define GMIO_STLA_TRIANGLE_RAWSIZE (4 * GMIO_STL_COORDS_RAWSIZE)

/*! Compact size of a gmio_stl_triangle_t object for STL binary format */
#define GMIO_STLB_TRIANGLE_RAWSIZE (GMIO_STLA_TRIANGLE_RAWSIZE + sizeof(uint16_t))

#endif /* GMIO_LIBSTL_TRIANGLE_H */
