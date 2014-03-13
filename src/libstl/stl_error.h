#ifndef FOUG_LIBSTL_STL_ERROR_H
#define FOUG_LIBSTL_STL_ERROR_H

/*! A byte-mask to tag(identify) STL-specific error codes */
#define FOUG_STL_ERROR_TAG  0x11000000

/*! This enum defines the various error codes reported by STL read/write functions */
enum foug_stl_error
{
  /*! Common STL write error indicating foug_stl_geom::get_triangle_func() pointer is NULL */
  FOUG_STL_WRITE_NULL_GET_TRIANGLE_FUNC_ERROR = FOUG_STL_ERROR_TAG + 1,

  /* Specific error codes returned by STL_ascii read function */

  /*! Parsing error occured in foug_stla_read() due to malformed STL ascii input */
  FOUG_STLA_READ_PARSE_ERROR = FOUG_STL_ERROR_TAG + 100,

  /*! Invalid max number of decimal significants digits for foug_stla_write(), must be in [1..9] */
  FOUG_STLA_WRITE_INVALID_REAL32_PREC_ERROR = FOUG_STL_ERROR_TAG + 101,

  /* Specific error codes returned by STL_binary read/write functions */

  /*! The byte order argument supplied is not supported, must be little or big endian */
  FOUG_STLB_UNSUPPORTED_BYTE_ORDER_ERROR = FOUG_STL_ERROR_TAG + 300,

  /*! Error occured when reading header data in foug_stlb_read() */
  FOUG_STLB_READ_HEADER_WRONG_SIZE_ERROR = FOUG_STL_ERROR_TAG + 301,

  /*! Error occured when reading facet count in foug_stlb_read() */
  FOUG_STLB_READ_FACET_COUNT_ERROR = FOUG_STL_ERROR_TAG + 302
};

typedef enum foug_stl_error  foug_stl_error_t;

#endif /* FOUG_LIBSTL_STL_ERROR_H */
