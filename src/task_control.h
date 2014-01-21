#ifndef FOUG_C_TASK_CONTROL_H
#define FOUG_C_TASK_CONTROL_H

#include "global.h"
#include "memory.h"

/* foug_task_control */
typedef struct foug_task_control foug_task_control_t;
struct foug_task_control
{
  void* cookie;
  foug_bool_t (*handle_progress_func)(foug_task_control_t*, uint8_t);
};

FOUG_LIB_EXPORT
foug_bool_t foug_task_control_handle_progress(foug_task_control_t* ctrl, uint8_t progress_pc);

FOUG_LIB_EXPORT
uint8_t foug_percentage(size_t range_min, size_t range_max, size_t value);

#endif /* FOUG_C_TASK_CONTROL_H */
