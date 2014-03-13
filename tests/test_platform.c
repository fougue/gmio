#include "utest_lib.h"

#include "../src/datax_core/global.h"
#include "../src/datax_stl/stl_triangle.h"

#include <stddef.h>

const char* test_platform__alignment()
{
  UTEST_ASSERT(offsetof(foug_stl_coords_t, x) == 0);
  UTEST_ASSERT(offsetof(foug_stl_coords_t, y) == 4);
  UTEST_ASSERT(offsetof(foug_stl_coords_t, z) == 8);
  UTEST_ASSERT(sizeof(foug_stl_coords_t) == FOUG_STL_COORDS_RAWSIZE);
  
  UTEST_ASSERT(offsetof(foug_stl_triangle_t, normal) == 0);
  UTEST_ASSERT(offsetof(foug_stl_triangle_t, v1) == FOUG_STL_COORDS_RAWSIZE);
  UTEST_ASSERT(offsetof(foug_stl_triangle_t, v2) == 2*FOUG_STL_COORDS_RAWSIZE);
  UTEST_ASSERT(offsetof(foug_stl_triangle_t, v3) == 3*FOUG_STL_COORDS_RAWSIZE);
  UTEST_ASSERT(offsetof(foug_stl_triangle_t, attribute_byte_count) == 4*FOUG_STL_COORDS_RAWSIZE);
  UTEST_ASSERT(sizeof(foug_stl_triangle_t) >= FOUG_STLB_TRIANGLE_RAWSIZE);

  return NULL;
}

const char* test_platform__global_h()
{
  UTEST_ASSERT(sizeof(int8_t) == 1);
  UTEST_ASSERT(sizeof(uint8_t) == 1);
  
  UTEST_ASSERT(sizeof(int16_t) == 2);
  UTEST_ASSERT(sizeof(uint16_t) == 2);
  
  UTEST_ASSERT(sizeof(int32_t) == 4);
  UTEST_ASSERT(sizeof(uint32_t) == 4);

  UTEST_ASSERT(sizeof(foug_real32_t) == 4);
  UTEST_ASSERT(sizeof(foug_real64_t) == 8);
  
  return NULL;
}

const char* all_tests()
{
  UTEST_SUITE_START();
  UTEST_RUN(test_platform__alignment);
  UTEST_RUN(test_platform__global_h);
  return NULL;
}

UTEST_MAIN(all_tests)
