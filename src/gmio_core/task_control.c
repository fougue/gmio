#include "task_control.h"

#include <stddef.h>

gmio_bool_t gmio_task_control_is_stop_requested(const gmio_task_control_t* task_ctrl)
{
  if (task_ctrl != NULL && task_ctrl->is_stop_requested_func != NULL)
    return task_ctrl->is_stop_requested_func(task_ctrl->cookie);
  return GMIO_FALSE;
}
