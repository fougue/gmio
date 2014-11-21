#ifndef GMIO_TRANSFER_H
#define GMIO_TRANSFER_H

#include "global.h"
#include "stream.h"
#include "task_control.h"

/*! Defines objects required for any transfer(read/write) operation */
struct gmio_transfer
{
  /*! The stream object to be used for I/O */
  gmio_stream_t       stream;

  /*! The optional object used to control execution of the transfer */
  gmio_task_control_t task_control;

  /*! Pointer on a memory buffer used by the transfer for stream operations */
  void*               buffer;

  /*! Size (in bytes) of the memory buffer */
  size_t              buffer_size;
};

typedef struct gmio_transfer gmio_transfer_t;

#endif /* GMIO_TRANSFER_H */
