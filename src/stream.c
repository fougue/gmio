#include "stream.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*!
 * \brief Installs a null stream
 */
void foug_stream_set_null(foug_stream_t* stream)
{
  memset(stream, 0, sizeof(foug_stream_t));
}

static foug_bool_t foug_stream_stdio_at_end(foug_stream_t* stream)
{
  return feof((FILE*) stream->cookie);
}

static int32_t foug_stream_stdio_error(foug_stream_t* stream)
{
  return ferror((FILE*) stream->cookie);
}

static size_t foug_stream_stdio_read(foug_stream_t* stream,
                                     void* ptr,
                                     size_t item_size,
                                     size_t item_count)
{
  return fread(ptr, item_size, item_count, (FILE*) stream->cookie);
}

static size_t foug_stream_stdio_write(foug_stream_t* stream,
                                      const void* ptr,
                                      size_t item_size,
                                      size_t item_count)
{
  return fwrite(ptr, item_size, item_count, (FILE*) stream->cookie);
}

/*!
 * \brief Configures \p stream for standard FILE*
 */
void foug_stream_set_stdio(foug_stream_t* stream, FILE* file)
{
  stream->cookie = file;
  stream->at_end_func = foug_stream_stdio_at_end;
  stream->error_func = foug_stream_stdio_error;
  stream->read_func = foug_stream_stdio_read;
  stream->write_func = foug_stream_stdio_write;
}

/*!
 * \brief Returns true if the current read and write position is at the end of the stream
 */
foug_bool_t foug_stream_at_end(foug_stream_t* stream)
{
  if (stream != NULL && stream->at_end_func != NULL)
    return stream->at_end_func(stream);
  return 0;
}

/*!
 * \brief Checks error indicator
 *
 * Checks if the error indicator associated with \p stream is set, returning a value different from
 * zero if it is.
 *
 * This indicator is generally set by a previous operation on the \p stream that failed.
 */
int foug_stream_error(foug_stream_t* stream)
{
  if (stream != NULL && stream->error_func != NULL)
    return stream->error_func(stream);
  return 0;
}

/*!
 * \brief Reads block of data from stream
 *
 * Reads an array of \p item_count elements, each one with a \p item_size of size bytes, from the
 * \p stream and stores them in the block of memory specified by \p ptr.
 *
 * The total amount of bytes read if successful is (item_size * item_count).
 *
 * \return The total number of elements successfully read is returned.
 *         If this number differs from the \p item_count argument, either a reading error occurred
 *         or the end-of-file was reached while reading. In both cases, the proper indicator is set,
 *         which can be checked with foug_stream_error() and foug_stream_at_end(), respectively.
 *         If either \p item_size or \p item_count is zero, the function returns zero and both the
 *         stream state and the content pointed by \p ptr remain unchanged.
 */
size_t foug_stream_read(foug_stream_t* stream, void *ptr, size_t item_size, size_t item_count)
{
  if (stream != NULL && stream->read_func != NULL)
    return stream->read_func(stream, ptr, item_size, item_count);
  return 0;
}

/*!
 * \brief Writes block of data to stream
 *
 *  Writes an array of \p item_count elements, each one with a \p item_size of size bytes, from the
 *  block of memory pointed by \p ptr to the current position in the \p stream.
 *
 *  The total amount of bytes written is (item_size * item_count).
 */
size_t foug_stream_write(foug_stream_t* stream, const void *ptr, size_t item_size, size_t item_count)
{
  if (stream != NULL && stream->write_func != NULL)
    return stream->write_func(stream, ptr, item_size, item_count);
  return 0;
}
