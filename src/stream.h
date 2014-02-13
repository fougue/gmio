#ifndef FOUG_C_STREAM_H
#define FOUG_C_STREAM_H

#include "global.h"
#include <stdio.h>

/*! Stream that can get input from an arbitrary data source or can write output to an arbitrary
 *  data sink
 */
struct foug_stream
{
  /*! Opaque pointer on the user stream, passed as first argument to hook functions */
  void* cookie;

  /*! Pointer on a function that checks end-of-stream indicator */
  foug_bool_t (*at_end_func)(void* cookie);

  /*! Pointer on a function that checks error indicator */
  int         (*error_func)(void* cookie);

  /*! Pointer on a function that reads block of data from stream */
  size_t      (*read_func)(void* cookie, void* ptr, size_t size, size_t count);

  /*! Pointer on a function that writes block of data to stream */
  size_t      (*write_func)(void* cookie, const void* ptr, size_t size, size_t count);
};

typedef struct foug_stream foug_stream_t;

/* Initialization */

/*! Installs a null stream */
FOUG_LIB_EXPORT void foug_stream_set_null(foug_stream_t* stream);

/*! Configures \p stream for standard FILE* (cookie will hold \p file) */
FOUG_LIB_EXPORT void foug_stream_set_stdio(foug_stream_t* stream, FILE* file);

/* Services */

/*! Safe and convenient function for foug_stream::at_end_func() */
FOUG_LIB_EXPORT foug_bool_t foug_stream_at_end(foug_stream_t* stream);

/*! Safe and convenient function for foug_stream::error_func() */
FOUG_LIB_EXPORT int foug_stream_error(foug_stream_t* stream);

/*! Safe and convenient function for foug_stream::read_func() */
FOUG_LIB_EXPORT size_t foug_stream_read(foug_stream_t* stream,
                                        void* ptr,
                                        size_t size,
                                        size_t count);

/*! Safe and convenient function for foug_stream::write_func() */
FOUG_LIB_EXPORT size_t foug_stream_write(foug_stream_t* stream,
                                         const void* ptr,
                                         size_t size,
                                         size_t count);

#endif /* FOUG_C_STREAM_H */
