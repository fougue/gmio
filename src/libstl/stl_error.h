#ifndef FOUG_LIBSTL_STL_ERROR_H
#define FOUG_LIBSTL_STL_ERROR_H

#define FOUG_STL_ERROR_TAG  0x11000000

enum foug_stl_rw_error
{
  FOUG_STL_WRITE_NULL_GET_TRIANGLE_FUNC_ERROR = FOUG_STL_ERROR_TAG + 1,

  /* Specific error codes returned by STL_ascii read function */
  FOUG_STLA_READ_PARSE_ERROR = FOUG_STL_ERROR_TAG + 100,
  FOUG_STLA_WRITE_INVALID_REAL32_PREC_ERROR = FOUG_STL_ERROR_TAG + 101,

  /* Specific error codes returned by STL_binary read/write functions */
  FOUG_STLB_UNSUPPORTED_BYTE_ORDER_ERROR = FOUG_STL_ERROR_TAG + 300,
  FOUG_STLB_READ_HEADER_WRONG_SIZE_ERROR = FOUG_STL_ERROR_TAG + 301,
  FOUG_STLB_READ_FACET_COUNT_ERROR = FOUG_STL_ERROR_TAG + 302
};

typedef enum foug_stl_rw_error foug_stl_rw_error_t;

#endif /* FOUG_LIBSTL_STL_ERROR_H */
