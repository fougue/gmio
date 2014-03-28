#ifndef GMIO_STREAM_BUFFER_H
#define GMIO_STREAM_BUFFER_H

#include "../src/gmio_core/stream.h"

typedef struct
{
  void*  ptr;
  size_t len;
  size_t pos;
} gmio_buffer_t;

void gmio_stream_set_buffer(gmio_stream_t* stream, gmio_buffer_t* buff);

#endif /* GMIO_STREAM_BUFFER_H */
