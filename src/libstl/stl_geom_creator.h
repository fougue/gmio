#ifndef FOUG_LIBSTL_STL_GEOM_CREATOR_H
#define FOUG_LIBSTL_STL_GEOM_CREATOR_H

#include "stl_global.h"
#include "stl_triangle.h"

/*! Provides an interface for the creation of the underlying(hidden) user mesh */
struct foug_stl_geom_creator
{
  /*! Opaque pointer on the user mesh, passed as first argument to hook functions */
  void* cookie;

  /* All function pointers are optional (ie can be set to NULL) */

  /*! Pointer on a function that handles declaration of a solid of name \p solid_name .
   *
   *  This optional function is useful only with STL ascii (ie. foug_stla_read())
   */
  void (*ascii_begin_solid_func)(void* cookie, const char* solid_name);

  /*! Pointer on a function that handles declaration of a mesh with \p tri_count number of triangles.
   *
   *  This optional function is useful only with STL binary (ie. foug_stlb_read()).
   *
   *  The argument \p header contains the header data(80 bytes)
   */
  void (*binary_begin_solid_func)(void* cookie, uint32_t tri_count, const uint8_t* header);

  /*! Pointer on a function that adds a triangle to the user mesh.
   *
   *  The argument \p triangle is the triangle to be added, note that
   *  foug_stl_triangle_t::attribute_byte_count is meaningless for STL ascii.
   *
   *  The argument \p tri_id is the index of the mesh triangle
   */
  void (*add_triangle_func)(void* cookie, uint32_t tri_id, const foug_stl_triangle_t* triangle);

  /*! Pointer on a function that finalizes creation of the user mesh.
   *
   *  This optional function is called at the end of the read process, ie. after all triangles have
   *  been added
   */
  void (*end_solid_func)(void* cookie);
};

typedef struct foug_stl_geom_creator foug_stl_geom_creator_t;

#endif /* FOUG_LIBSTL_STL_GEOM_CREATOR_H */
