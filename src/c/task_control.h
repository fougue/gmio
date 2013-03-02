#ifndef FOUG_C_TASK_CONTROL_H
#define FOUG_C_TASK_CONTROL_H

#include "global.h"
#include "memory.h"

/* foug_task_progress */
typedef struct foug_task_progress
{
  foug_real32_t range_min;
  foug_real32_t range_max;
  foug_real32_t value;
} foug_task_progress_t;

/* Progress */
FOUG_LIB_EXPORT foug_real32_t foug_task_progress_get_value_pc(const foug_task_progress_t* progress);

/* foug_task_control */
typedef struct foug_task_control foug_task_control_t;
struct foug_task_control
{
  foug_bool_t is_stop_requested;
  void* cookie;
  void (*handle_stop_func)(foug_task_control_t*);
  void (*handle_progress_update_func)(foug_task_control_t*, const foug_task_progress_t*);
};

FOUG_LIB_EXPORT void foug_task_control_set_progress(foug_task_control_t* ctrl,
                                                    foug_task_progress_t* progress,
                                                    foug_real32_t value);

/* Task stop */
FOUG_LIB_EXPORT void foug_task_control_async_stop(foug_task_control_t* ctrl);
FOUG_LIB_EXPORT void foug_task_control_handle_stop(foug_task_control_t* ctrl);

#endif /* FOUG_C_TASK_CONTROL_H */
