#ifndef FOUG_C_TASK_CONTROL_H
#define FOUG_C_TASK_CONTROL_H

#include "foug_global.h"
#include "memory.h"

/* foug_task_control : opaque structure */
typedef struct _internal_foug_task_control foug_task_control_t;

/* foug_task_control_manip */
typedef void (*foug_task_control_handle_stop_func)(foug_task_control_t*);
typedef void (*foug_task_control_handle_progress_update_func)(foug_task_control_t*);
typedef struct
{
  foug_task_control_handle_stop_func handle_stop_func;
  foug_task_control_handle_progress_update_func handle_progress_update_func;
} foug_task_control_manip_t;

void foug_task_control_manip_init(foug_task_control_manip_t* manip);

foug_task_control_t* foug_task_control_create(foug_malloc_func_t func,
                                              void* data, foug_task_control_manip_t manip);

/* Range */
foug_real32_t foug_task_control_get_range_min(const foug_task_control_t* ctrl);
foug_real32_t foug_task_control_get_range_max(const foug_task_control_t* ctrl);
void foug_task_control_set_range(foug_task_control_t* ctrl, foug_real32_t min, foug_real32_t max);

/* Step id */
int32_t foug_task_control_get_step_id(const foug_task_control_t* ctrl);
void foug_task_control_set_step_id(foug_task_control_t* ctrl, int32_t step_id);

/* Progress */
foug_real32_t foug_task_control_get_progress_as_pc(const foug_task_control_t* ctrl);
foug_real32_t foug_task_control_get_progress(const foug_task_control_t* ctrl);
void foug_task_control_set_progress(foug_task_control_t* ctrl, foug_real32_t v);

foug_real32_t foug_task_control_get_progress_update_threshold(const foug_task_control_t* ctrl);
void foug_task_control_set_progress_update_threshold(foug_task_control_t* ctrl, foug_real32_t v);

/* Reset */
void foug_task_control_reset(foug_task_control_t* ctrl);

/* Task stop */
void foug_task_control_async_stop(foug_task_control_t* ctrl);
void foug_task_control_handle_stop(foug_task_control_t* ctrl);
foug_bool_t foug_task_control_is_stop_requested(const foug_task_control_t* ctrl);

/* Cookie */
void* foug_task_control_get_cookie(const foug_task_control_t* ctrl);

#endif /* FOUG_C_TASK_CONTROL_H */
