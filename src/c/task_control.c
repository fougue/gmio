#include "task_control.h"

#include <math.h>
#include <string.h>

/* foug_task_control */

foug_real32_t foug_task_progress_get_value_pc(const foug_task_progress_t *progress)
{
  if (progress == NULL)
    return 0.f;
  return fabs((progress->value - progress->range_min) / (progress->range_max - progress->range_min));
}

FOUG_LIB_EXPORT void foug_task_control_set_progress(foug_task_control_t* ctrl,
                                                    foug_task_progress_t *progress,
                                                    foug_real32_t value)
{
  progress->value = value;
  if (ctrl->handle_progress_update_func != NULL)
    ctrl->handle_progress_update_func(ctrl, progress);
}

/* Task stop */

void foug_task_control_async_stop(foug_task_control_t* ctrl)
{
  if (ctrl != NULL)
    ctrl->is_stop_requested = 1;
}

void foug_task_control_handle_stop(foug_task_control_t* ctrl)
{
  if (ctrl != NULL) {
    ctrl->is_stop_requested = 0;
    if (ctrl->handle_stop_func != NULL)
      ctrl->handle_stop_func(ctrl);
  }
}
