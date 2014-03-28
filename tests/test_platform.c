#include "utest_lib.h"

#include "../src/gmio_core/global.h"
#include "../src/gmio_stl/stl_triangle.h"

#include <stddef.h>

const char* test_platform__alignment()
{
  UTEST_ASSERT(offsetof(gmio_stl_coords_t, x) == 0);
  UTEST_ASSERT(offsetof(gmio_stl_coords_t, y) == 4);
  UTEST_ASSERT(offsetof(gmio_stl_coords_t, z) == 8);
  UTEST_ASSERT(sizeof(gmio_stl_coords_t) == GMIO_STL_COORDS_RAWSIZE);
  
  UTEST_ASSERT(offsetof(gmio_stl_triangle_t, normal) == 0);
  UTEST_ASSERT(offsetof(gmio_stl_triangle_t, v1) == GMIO_STL_COORDS_RAWSIZE);
  UTEST_ASSERT(offsetof(gmio_stl_triangle_t, v2) == 2*GMIO_STL_COORDS_RAWSIZE);
  UTEST_ASSERT(offsetof(gmio_stl_triangle_t, v3) == 3*GMIO_STL_COORDS_RAWSIZE);
  UTEST_ASSERT(offsetof(gmio_stl_triangle_t, attribute_byte_count) == 4*GMIO_STL_COORDS_RAWSIZE);
  UTEST_ASSERT(sizeof(gmio_stl_triangle_t) >= GMIO_STLB_TRIANGLE_RAWSIZE);

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

  UTEST_ASSERT(sizeof(gmio_real32_t) == 4);
  UTEST_ASSERT(sizeof(gmio_real64_t) == 8);
  
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
