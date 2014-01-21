#ifndef FOUG_C_TRANSFER_H
#define FOUG_C_TRANSFER_H

#include "global.h"
#include "stream.h"
#include "task_control.h"

typedef struct foug_transfer
{
  foug_stream_t       stream;
  foug_task_control_t task_control;
  void*               buffer;
  size_t              buffer_size;
} foug_transfer_t;

#endif /* FOUG_C_TRANSFER_H */
