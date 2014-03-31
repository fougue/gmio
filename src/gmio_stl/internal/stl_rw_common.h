#ifndef GMIO_INTERNAL_STL_RW_COMMON_H
#define GMIO_INTERNAL_STL_RW_COMMON_H

#include "../../gmio_core/global.h"
#include "../../gmio_core/endian.h"
#include "../../gmio_core/transfer.h"
#include "../stl_geom.h"
#include "../stl_triangle.h"

struct gmio_stlb_readwrite_helper
{
  uint32_t facet_count;
  uint32_t i_facet_offset;
  void   (*fix_endian_func)(gmio_stl_triangle_t* tri);
};
typedef struct gmio_stlb_readwrite_helper  gmio_stlb_readwrite_helper_t;

gmio_bool_t gmio_check_transfer(int* error, const gmio_transfer_t* trsf);

gmio_bool_t gmio_stl_check_geom(int* error, const gmio_stl_geom_t* geom);

gmio_bool_t gmio_stlb_check_params(int* error,
                                   const gmio_transfer_t* trsf,
                                   gmio_endianness_t byte_order);

#endif /* GMIO_INTERNAL_STLB_RW_COMMON_H */
