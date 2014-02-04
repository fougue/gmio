#ifndef FOUG_STREAM_BUFFER_H
#define FOUG_STREAM_BUFFER_H

#include "../src/stream.h"

typedef struct
{
  void*  ptr;
  size_t len;
  size_t pos;
} foug_buffer_t;

void foug_stream_set_buffer(foug_stream_t* stream, foug_buffer_t* buff);

#endif /* FOUG_STREAM_BUFFER_H */
