#include "stlb_byte_swap.h"

#include "../byte_swap.h"

void foug_stlb_triangle_bswap(foug_stlb_triangle_t* triangle)
{
  int i;
  uint32_t* uintcoord_ptr = (uint32_t*)&(triangle->data.normal.x);

  for (i = 0; i < 12; ++i)
    *(uintcoord_ptr + i) = foug_uint32_bswap(*(uintcoord_ptr + i));
  if (triangle->attribute_byte_count != 0)
    triangle->attribute_byte_count = foug_uint16_bswap(triangle->attribute_byte_count);
}
