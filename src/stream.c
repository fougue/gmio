#include "stream.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*! \struct foug_stream
 *
 *  \details This is pretty much the same as [custom streams]
 *  (http://www.gnu.org/software/libc/manual/html_mono/libc.html#Custom-Streams) in the GNU C Library
 *
 * It uses a cookie being basically an opaque pointer on a hidden data type.\n The custom stream is
 * implemented by defining hook functions that know how to read/write the data.
 */

/*! \var foug_bool_t (*foug_stream::at_end_func)(void*)
 *
 *  Checks whether the end-of-stream indicator associated with stream pointed by \p cookie is set,
 *  returning FOUG_TRUE if is.
 *
 *  The function should behaves like C standard [feof()]
 *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/feof.html)
 */

/*! \var int (*foug_stream::error_func)(void*)
 *
 *  Checks if the error indicator associated with stream pointed by \p cookie is set, returning
 *  a value different from zero if it is.
 *
 *  The function should behaves like C standard [ferror()]
 *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/ferror.html)
 */

/*! \var size_t (*foug_stream::read_func)(void*, void*, size_t, size_t)
 *
 *  Reads an array of \p count elements, each one with a size of \p size bytes, from the stream
 *  pointed by \p cookie and stores them in the block of memory specified by \p ptr
 *
 *  The function should behaves like C standard [fread()]
 *  (http://pubs.opengroup.org/onlinepubs/007904975/functions/fread.html)
 *
 *  \returns The total number of elements successfully read
 */

/*! \var size_t (*foug_stream::write_func)(void*, const void*, size_t, size_t);
 *
 *  Writes an array of \p count elements, each one with a size of \p size bytes, from the
 *  block of memory pointed by \p ptr to the current position in the stream pointed by \p cookie
 *
 *  The function should behaves like C standard [fwrite()]
 * (http://pubs.opengroup.org/onlinepubs/007904975/functions/fwrite.html)
 *
 *  \returns The total number of elements successfully written
 */



void foug_stream_set_null(foug_stream_t* stream)
{
  memset(stream, 0, sizeof(foug_stream_t));
}

static foug_bool_t foug_stream_stdio_at_end(void* cookie)
{
  return feof((FILE*) cookie);
}

static int foug_stream_stdio_error(void* cookie)
{
  return ferror((FILE*) cookie);
}

static size_t foug_stream_stdio_read(void* cookie,
                                     void* ptr,
                                     size_t item_size,
                                     size_t item_count)
{
  return fread(ptr, item_size, item_count, (FILE*) cookie);
}

static size_t foug_stream_stdio_write(void* cookie,
                                      const void* ptr,
                                      size_t item_size,
                                      size_t item_count)
{
  return fwrite(ptr, item_size, item_count, (FILE*) cookie);
}

void foug_stream_set_stdio(foug_stream_t* stream, FILE* file)
{
  stream->cookie = file;
  stream->at_end_func = foug_stream_stdio_at_end;
  stream->error_func = foug_stream_stdio_error;
  stream->read_func = foug_stream_stdio_read;
  stream->write_func = foug_stream_stdio_write;
}

/*! \details Basically the same as :
 *  \code
 *    stream->at_end_func(stream->cookie)
 *  \endcode
 */
foug_bool_t foug_stream_at_end(foug_stream_t* stream)
{
  if (stream != NULL && stream->at_end_func != NULL)
    return stream->at_end_func(stream->cookie);
  return 0;
}

/*! \details Basically the same as :
 *  \code
 *    stream->error_func(stream->cookie)
 *  \endcode
 */
int foug_stream_error(foug_stream_t* stream)
{
  if (stream != NULL && stream->error_func != NULL)
    return stream->error_func(stream->cookie);
  return 0;
}

/*! \details Basically the same as :
 *  \code
 *    stream->read_func(stream->cookie)
 *  \endcode
 */
size_t foug_stream_read(foug_stream_t* stream, void *ptr, size_t size, size_t count)
{
  if (stream != NULL && stream->read_func != NULL)
    return stream->read_func(stream->cookie, ptr, size, count);
  return 0;
}

/*! \details Basically the same as :
 *  \code
 *    stream->write_func(stream->cookie)
 *  \endcode
 */
size_t foug_stream_write(foug_stream_t* stream, const void *ptr, size_t size, size_t count)
{
  if (stream != NULL && stream->write_func != NULL)
    return stream->write_func(stream->cookie, ptr, size, count);
  return 0;
}
