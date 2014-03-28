#include "stlb_byte_swap.h"

#include "../../gmio_core/internal/byte_swap.h"

void gmio_stl_triangle_bswap(gmio_stl_triangle_t* triangle)
{
  int i;
  uint32_t* uintcoord_ptr = (uint32_t*)&(triangle->normal.x);

  for (i = 0; i < 12; ++i)
    *(uintcoord_ptr + i) = gmio_uint32_bswap(*(uintcoord_ptr + i));
  if (triangle->attribute_byte_count != 0)
    triangle->attribute_byte_count = gmio_uint16_bswap(triangle->attribute_byte_count);
}
