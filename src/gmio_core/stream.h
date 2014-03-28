#ifndef GMIO_STREAM_H
#define GMIO_STREAM_H

#include "global.h"
#include <stdio.h>

GMIO_C_LINKAGE_BEGIN

/*! Stream that can get input from an arbitrary data source or can write output to an arbitrary
 *  data sink
 */
struct gmio_stream
{
  /*! Opaque pointer on the user stream, passed as first argument to hook functions */
  void* cookie;

  /*! Pointer on a function that checks end-of-stream indicator */
  gmio_bool_t (*at_end_func)(void* cookie);

  /*! Pointer on a function that checks error indicator */
  int         (*error_func)(void* cookie);

  /*! Pointer on a function that reads block of data from stream */
  size_t      (*read_func)(void* cookie, void* ptr, size_t size, size_t count);

  /*! Pointer on a function that writes block of data to stream */
  size_t      (*write_func)(void* cookie, const void* ptr, size_t size, size_t count);
};

typedef struct gmio_stream gmio_stream_t;

/* Initialization */

/*! Installs a null stream */
GMIO_LIB_EXPORT void gmio_stream_set_null(gmio_stream_t* stream);

/*! Configures \p stream for standard FILE* (cookie will hold \p file) */
GMIO_LIB_EXPORT void gmio_stream_set_stdio(gmio_stream_t* stream, FILE* file);

/* Services */

/*! Safe and convenient function for gmio_stream::at_end_func() */
GMIO_LIB_EXPORT gmio_bool_t gmio_stream_at_end(gmio_stream_t* stream);

/*! Safe and convenient function for gmio_stream::error_func() */
GMIO_LIB_EXPORT int gmio_stream_error(gmio_stream_t* stream);

/*! Safe and convenient function for gmio_stream::read_func() */
GMIO_LIB_EXPORT size_t gmio_stream_read(gmio_stream_t* stream,
                                        void* ptr,
                                        size_t size,
                                        size_t count);

/*! Safe and convenient function for gmio_stream::write_func() */
GMIO_LIB_EXPORT size_t gmio_stream_write(gmio_stream_t* stream,
                                         const void* ptr,
                                         size_t size,
                                         size_t count);

GMIO_C_LINKAGE_END

#endif /* GMIO_STREAM_H */
