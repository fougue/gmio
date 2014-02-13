#ifndef FOUG_C_STREAM_H
#define FOUG_C_STREAM_H

#include "global.h"
#include <stdio.h>

/*! Stream that can get input from an arbitrary data source or can write output to an arbitrary
 *  data sink.
 *
 *  This is pretty much the same as
 *  <a href="http://www.gnu.org/software/libc/manual/html_mono/libc.html#Custom-Streams">
 *  custom streams</a> in the GNU C Library
 *
 * It uses a cookie being basically an opaque pointer on a hidden data type. The custom stream is
 * implemented by defining hook functions that know how to read/write the data.
 *
 */
struct foug_stream
{
  /*! Opaque pointer on the user stream, passed as first argument to hook functions */
  void* cookie;

  /*! Pointer on a function that checks end-of-stream indicator.
   *
   *  Checks whether the end-of-stream indicator associated with stream pointed by \p cookie
   *  is set, returning FOUG_TRUE if is.
   *
   *  The function should behaves like C standard feof()
   */
  foug_bool_t (*at_end_func)(void* cookie);

  /*! Pointer on a function that checks error indicator.
   *
   *  Checks if the error indicator associated with stream pointed by \p cookie is set, returning
   *  a value different from zero if it is.
   *
   *  The function should behaves like C standard ferror()
   */
  int         (*error_func)(void* cookie);

  /*! Pointer on a function that reads block of data from stream.
   *
   *  \details Reads an array of \p count elements, each one with a size of \p size bytes, from the
   *  stream pointed by \p cookie and stores them in the block of memory specified by \p ptr
   *
   *  The function should behaves like C standard fread()
   *
   *  \returns The total number of elements successfully read
   */
  size_t      (*read_func)(void* cookie, void* ptr, size_t size, size_t count);

  /*! Pointer on a function that writes block of data to stream.
   *
   *  \details Writes an array of \p count elements, each one with a size of \p size bytes, from the
   *  block of memory pointed by \p ptr to the current position in the stream pointed by \p cookie
   *
   *  The function should behaves like C standard fwrite()
   *
   *  \returns The total number of elements successfully written
   */
  size_t      (*write_func)(void* cookie, const void* ptr, size_t size, size_t count);
};

/*! Convenient typedef for struct foug_stream */
typedef struct foug_stream foug_stream_t;

/* Initialization */

/*! Installs a null stream */
FOUG_LIB_EXPORT void foug_stream_set_null(foug_stream_t* stream);

/*! Configures \p stream for standard FILE* (cookie will hold \p file) */
FOUG_LIB_EXPORT void foug_stream_set_stdio(foug_stream_t* stream, FILE* file);

/* Services */

/*! Safe and convenient function for foug_stream::at_end_func().
 *
 *  Basically the same as : \code stream->at_end_func(stream->cookie) \endcode
 */
FOUG_LIB_EXPORT foug_bool_t foug_stream_at_end(foug_stream_t* stream);

/*! Safe and convenient function for foug_stream::error_func().
 *
 *  Basically the same as : \code stream->error_func(stream->cookie) \endcode
 */
FOUG_LIB_EXPORT int foug_stream_error(foug_stream_t* stream);

/*! Safe and convenient function for foug_stream::read_func().
 *
 *  Basically the same as : \code stream->read_func(stream->cookie) \endcode
 */
FOUG_LIB_EXPORT size_t foug_stream_read(foug_stream_t* stream,
                                        void* ptr,
                                        size_t size,
                                        size_t count);

/*! Safe and convenient function for foug_stream::write_func().
 *
 *  Basically the same as : \code stream->write_func(stream->cookie) \endcode
 */
FOUG_LIB_EXPORT size_t foug_stream_write(foug_stream_t* stream,
                                         const void* ptr,
                                         size_t size,
                                         size_t count);

#endif /* FOUG_C_STREAM_H */
