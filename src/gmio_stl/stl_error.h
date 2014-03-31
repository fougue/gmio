#ifndef GMIO_LIBSTL_STL_ERROR_H
#define GMIO_LIBSTL_STL_ERROR_H

/*! A byte-mask to tag(identify) STL-specific error codes */
#define GMIO_STL_ERROR_TAG  0x11000000

/*! This enum defines the various error codes reported by STL read/write functions */
enum gmio_stl_error
{
  /*! Common STL write error indicating gmio_stl_mesh::get_triangle_func() pointer is NULL */
  GMIO_STL_WRITE_NULL_GET_TRIANGLE_FUNC_ERROR = GMIO_STL_ERROR_TAG + 1,

  /* Specific error codes returned by STL_ascii read function */

  /*! Parsing error occured in gmio_stla_read() due to malformed STL ascii input */
  GMIO_STLA_READ_PARSE_ERROR = GMIO_STL_ERROR_TAG + 100,

  /*! Invalid max number of decimal significants digits for gmio_stla_write(), must be in [1..9] */
  GMIO_STLA_WRITE_INVALID_REAL32_PREC_ERROR = GMIO_STL_ERROR_TAG + 101,

  /* Specific error codes returned by STL_binary read/write functions */

  /*! The byte order argument supplied is not supported, must be little or big endian */
  GMIO_STLB_UNSUPPORTED_BYTE_ORDER_ERROR = GMIO_STL_ERROR_TAG + 300,

  /*! Error occured when reading header data in gmio_stlb_read() */
  GMIO_STLB_READ_HEADER_WRONG_SIZE_ERROR = GMIO_STL_ERROR_TAG + 301,

  /*! Error occured when reading facet count in gmio_stlb_read() */
  GMIO_STLB_READ_FACET_COUNT_ERROR = GMIO_STL_ERROR_TAG + 302
};

typedef enum gmio_stl_error  gmio_stl_error_t;

#endif /* GMIO_LIBSTL_STL_ERROR_H */
