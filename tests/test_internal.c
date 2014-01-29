#include "utest_lib.h"

#include "../src/internal/byte_swap.h"
#include "../src/internal/byte_codec.h"

const char* test_internal__byte_swap()
{
  UTEST_ASSERT(foug_uint16_bswap(0x1122) == 0x2211);
  UTEST_ASSERT(foug_uint32_bswap(0x11223344) == 0x44332211);
  return NULL;
}

const char* test_internal__byte_codec()
{
  { /* decode */
    const uint8_t data[] = { 0x11, 0x22, 0x33, 0x44 };
    UTEST_ASSERT(foug_decode_uint16_le(data) == 0x2211);
    UTEST_ASSERT(foug_decode_uint16_be(data) == 0x1122);
    UTEST_ASSERT(foug_decode_uint32_le(data) == 0x44332211);
    UTEST_ASSERT(foug_decode_uint32_be(data) == 0x11223344);
  }

  { /* encode */
    uint8_t data[] = { 0, 0, 0, 0 };
    foug_encode_uint16_le(0x1122, data);
    UTEST_ASSERT(data[0] == 0x22 && data[1] == 0x11);
    foug_encode_uint32_le(0x11223344, data);
    UTEST_ASSERT(data[0] == 0x44 && data[1] == 0x33 && data[2] == 0x22 && data[3] == 0x11);
    foug_encode_uint32_be(0x11223344, data);
    UTEST_ASSERT(data[3] == 0x44 && data[2] == 0x33 && data[1] == 0x22 && data[0] == 0x11);
  }

  return NULL;
}

const char* test_internal__ascii_parse()
{
}

const char* all_tests()
{
  UTEST_SUITE_START();
  UTEST_RUN(test_internal__byte_swap);
  UTEST_RUN(test_internal__byte_codec);
  return NULL;
}

UTEST_MAIN(all_tests)