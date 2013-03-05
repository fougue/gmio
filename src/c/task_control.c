#include "task_control.h"

#include <math.h>
#include <string.h>

foug_bool_t foug_task_control_handle_progress(foug_task_control_t* ctrl, uint8_t progress_pc)
{
  if (ctrl != NULL && ctrl->handle_progress_func != NULL)
    return ctrl->handle_progress_func(ctrl, progress_pc);
  return 1;
}

uint8_t foug_percentage(size_t range_min, size_t range_max, size_t value)
{
  if (value >= range_max)
    return 100;
  else if (value <= range_min)
    return 0;
  else if (range_min < range_max)
    return (value * 100) / (range_max - range_min);
  return 0;
}
