#ifndef FOUG_C_TASK_CONTROL_H
#define FOUG_C_TASK_CONTROL_H

#include "global.h"
#include "memory.h"

/* foug_task_control : opaque structure */
typedef struct _internal_foug_task_control foug_task_control_t;


/* foug_task_control_manip */
typedef struct foug_task_control_manip
{
  void (*handle_stop_func)(foug_task_control_t*);
  void (*handle_progress_update_func)(foug_task_control_t*);
} foug_task_control_manip_t;

FOUG_LIB_EXPORT void foug_task_control_manip_init(foug_task_control_manip_t* manip);

/* Creation */

FOUG_LIB_EXPORT foug_task_control_t* foug_task_control_create(foug_malloc_func_t func,
                                                              void* data,
                                                              foug_task_control_manip_t manip);

/* Range */

FOUG_LIB_EXPORT foug_real32_t foug_task_control_get_range_min(const foug_task_control_t* ctrl);
FOUG_LIB_EXPORT foug_real32_t foug_task_control_get_range_max(const foug_task_control_t* ctrl);

FOUG_LIB_EXPORT void foug_task_control_set_range(foug_task_control_t* ctrl,
                                                 foug_real32_t min,
                                                 foug_real32_t max);

/* Step id */

FOUG_LIB_EXPORT int32_t foug_task_control_get_step_id(const foug_task_control_t* ctrl);

FOUG_LIB_EXPORT void foug_task_control_set_step_id(foug_task_control_t* ctrl, int32_t step_id);

/* Progress */

FOUG_LIB_EXPORT foug_real32_t foug_task_control_get_progress_as_pc(const foug_task_control_t* ctrl);
FOUG_LIB_EXPORT foug_real32_t foug_task_control_get_progress(const foug_task_control_t* ctrl);

FOUG_LIB_EXPORT void foug_task_control_set_progress(foug_task_control_t* ctrl, foug_real32_t v);

FOUG_LIB_EXPORT
foug_real32_t foug_task_control_get_progress_update_threshold(const foug_task_control_t* ctrl);

FOUG_LIB_EXPORT void foug_task_control_set_progress_update_threshold(foug_task_control_t* ctrl,
                                                                     foug_real32_t v);

/* Reset */

FOUG_LIB_EXPORT void foug_task_control_reset(foug_task_control_t* ctrl);

/* Task stop */

FOUG_LIB_EXPORT void foug_task_control_async_stop(foug_task_control_t* ctrl);

FOUG_LIB_EXPORT void foug_task_control_handle_stop(foug_task_control_t* ctrl);

FOUG_LIB_EXPORT foug_bool_t foug_task_control_is_stop_requested(const foug_task_control_t* ctrl);

/* Cookie */

FOUG_LIB_EXPORT void* foug_task_control_get_cookie(const foug_task_control_t* ctrl);

FOUG_LIB_EXPORT void foug_task_control_set_cookie(foug_task_control_t* ctrl, void* data);

#endif /* FOUG_C_TASK_CONTROL_H */
