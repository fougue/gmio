#ifndef FOUG_LIBSTL_STL_ERROR_H
#define FOUG_LIBSTL_STL_ERROR_H

#define FOUG_STL_ERROR_TAG          0x11000000
/* Specific error codes returned by foug_stla_read() */
#define FOUG_STLA_READ_PARSE_ERROR  (FOUG_STL_ERROR_TAG + 1)

/* Specific error codes returned by foug_stla_write() */
#define FOUG_STLA_WRITE_NULL_GET_TRIANGLE_FUNC   (FOUG_STL_ERROR_TAG + 100)
#define FOUG_STLA_WRITE_INVALID_REAL32_PRECISION (FOUG_STL_ERROR_TAG + 101)

/* Specific error code common to foug_stlb_read() and foug_stlb_write() */
#define FOUG_STLB_READWRITE_UNSUPPORTED_BYTE_ORDER    (FOUG_STL_ERROR_TAG + 200)

/* Specific error codes returned by foug_stlb_read() */
#define FOUG_STLB_READ_HEADER_WRONG_SIZE_ERROR   (FOUG_STL_ERROR_TAG + 300)
#define FOUG_STLB_READ_FACET_COUNT_ERROR         (FOUG_STL_ERROR_TAG + 301)
#define FOUG_STLB_READ_UNSUPPORTED_BYTE_ORDER    (FOUG_STL_ERROR_TAG + 302)

/* Specific error codes returned by foug_stlb_write() */
#define FOUG_STLB_WRITE_NULL_GET_TRIANGLE_FUNC       (FOUG_STL_ERROR_TAG + 400)

#endif /* FOUG_LIBSTL_STL_ERROR_H */
