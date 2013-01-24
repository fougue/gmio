#include "task_control.h"

#include <math.h>
#include <string.h>

/* foug_task_control */

struct _internal_foug_task_control
{
  foug_real32_t range_min;
  foug_real32_t range_max;
  foug_real32_t range_length;
  int32_t step_id;
  foug_real32_t progress_value;
  foug_real32_t progress_threshold;
  foug_bool_t is_stop_requested;
  void* cookie;
  foug_task_control_manip_t manip;
};

foug_task_control_t* foug_task_control_create(foug_malloc_func_t func,
                                              void* data, foug_task_control_manip_t manip)
{
  foug_task_control_t* ctrl;

  if (func == NULL)
    return NULL;

  ctrl = (*func)(sizeof(struct _internal_foug_task_control));
  if (ctrl != 0) {
    ctrl->range_min = -1.f;
    ctrl->range_max = -2.f;
    ctrl->range_length = -0.f;
    ctrl->step_id = -1;
    ctrl->progress_value = -1.f;
    ctrl->progress_threshold = 0.01f; /* Notifies each percent only */
    ctrl->is_stop_requested = 0;
    ctrl->cookie = data;
    ctrl->manip = manip;
  }
  return ctrl;
}

/* foug_task_control_manip */

void foug_task_control_manip_init(foug_task_control_manip_t* manip)
{
  if (manip != NULL)
    memset(manip, 0, sizeof(foug_task_control_manip_t));
}

/* Range */

foug_real32_t foug_task_control_get_range_min(const foug_task_control_t* ctrl)
{
  return ctrl != NULL ? ctrl->range_min : -1.f;
}

foug_real32_t foug_task_control_get_range_max(const foug_task_control_t* ctrl)
{
  return ctrl != NULL ? ctrl->range_max : -2.f;
}

void foug_task_control_set_range(foug_task_control_t* ctrl, foug_real32_t min, foug_real32_t max)
{
  if (ctrl != NULL) {
    ctrl->range_min = min;
    ctrl->range_max = max;
    ctrl->range_length = max - min;
  }
}

/* Step id */

int32_t foug_task_control_get_step_id(const foug_task_control_t* ctrl)
{
  return ctrl != NULL ? ctrl->step_id : -1;
}

void foug_task_control_set_step_id(foug_task_control_t* ctrl, int32_t step_id)
{
  if (ctrl != NULL)
    ctrl->step_id = step_id;
}

/* Progress */
foug_real32_t foug_task_control_get_progress_as_pc(const foug_task_control_t* ctrl)
{
  if (ctrl == NULL)
    return 0.f;
  return fabsf((ctrl->progress_value - ctrl->range_min) / ctrl->range_length);
}

foug_real32_t foug_task_control_get_progress(const foug_task_control_t* ctrl)
{
  return ctrl != NULL ? ctrl->progress_value : 0.f;
}

void foug_task_control_set_progress(foug_task_control_t* ctrl, foug_real32_t v)
{
  if (ctrl == NULL)
    return;

  if (fabs(v - ctrl->progress_value) > fabs(ctrl->progress_threshold * ctrl->range_length)) {
    ctrl->progress_value = v;
    if (ctrl->manip.handle_progress_update_func != NULL)
      (*(ctrl->manip.handle_progress_update_func))(ctrl);
  }
}

foug_real32_t foug_task_control_get_progress_update_threshold(const foug_task_control_t* ctrl)
{
  return ctrl != NULL ? ctrl->progress_threshold : 0.01f;
}

void foug_task_control_set_progress_update_threshold(foug_task_control_t* ctrl, foug_real32_t v)
{
  if (ctrl != NULL)
    ctrl->progress_threshold = v;
}

/* Reset */

void foug_task_control_reset(foug_task_control_t* ctrl)
{
  if (ctrl != NULL) {
    ctrl->step_id = -1;
    ctrl->progress_value = -1.f;
    ctrl->range_min = -1.f;
    ctrl->range_max = -2.f;
    ctrl->is_stop_requested = 0;
  }
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
    if (ctrl->manip.handle_stop_func != NULL)
      (*(ctrl->manip.handle_stop_func))(ctrl);
  }
}

foug_bool_t foug_task_control_is_stop_requested(const foug_task_control_t* ctrl)
{
  return ctrl != NULL ? ctrl->is_stop_requested : 0;
}

/* Cookie */

void* foug_task_control_get_cookie(const foug_task_control_t* ctrl)
{
  return ctrl != NULL ? ctrl->cookie : NULL;
}
