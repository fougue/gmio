#ifndef FOUG_C_LIBSTL_STLB_H
#define FOUG_C_LIBSTL_STLB_H

#include "triangle.h"
#include "../foug_global.h"
#include "../stream.h"
#include "../task_control.h"

enum { foug_stlb_header_size = 80 };
typedef foug_uint8 foug_stlb_header_t[foug_stlb_header_size];

/* foug_stlb_geom : opaque structure */
typedef struct _internal_foug_stlb_geom foug_stlb_geom_t;
typedef void (*foug_stlb_geom_input_process_header_func_t)(foug_stlb_geom_t,
                                                           const foug_stlb_header_t);
typedef void (*foug_stlb_geom_input_begin_triangles_func_t)(foug_stlb_geom_t, foug_uint32);
typedef void (*foug_stlb_geom_input_process_next_triangle_func_t)(foug_stlb_geom_t,
                                                                  const foug_stl_triangle_t*,
                                                                  foug_uint16);
typedef void (*foug_stlb_geom_input_end_triangles_func_t)(foug_stlb_geom_t);

typedef struct foug_stlb_geom_input_manip
{
  foug_stlb_geom_input_process_header_func_t process_header_func;
  foug_stlb_geom_input_begin_triangles_func_t begin_triangles_func;
  foug_stlb_geom_input_process_next_triangle_func_t process_next_triangle_func;
  foug_stlb_geom_input_end_triangles_func_t end_triangles_func;
} foug_stlb_geom_input_manip_t;

/*
typedef struct foug_stlb_read_args
{
  void* stream;
  foug_stream_manip_t stream_manip;

  foug_stlb_geom_t geom;
  foug_stlb_geom_input_manip_t geom_manip;
} foug_stlb_read_args_t;

int foug_stlb_read(foug_stlb_geom_t geom, foug_stlb_read_args_t args);
*/

/*class FOUG_STL_EXPORT AbstractGeometryExtraData
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
};*/

#endif /* FOUG_C_LIBSTL_STLB_H */
