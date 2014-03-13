#ifndef FOUG_C_TRANSFER_H
#define FOUG_C_TRANSFER_H

#include "global.h"
#include "stream.h"
#include "task_control.h"

/*! Defines data required for any transfer(read/write) operation */
struct foug_transfer
{
  /*! The stream to be used for I/O */
  foug_stream_t       stream;

  /*! The optional control object used to handle progress of the transfer */
  foug_task_control_t task_control;

  /*! Pointer on a user memory area used by the transfer as a buffer for stream optimization */
  void*               buffer;

  /*! Size (in bytes) of the buffer memoru */
  size_t              buffer_size;
};

typedef struct foug_transfer foug_transfer_t;

#endif /* FOUG_C_TRANSFER_H */
