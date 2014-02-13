#ifndef FOUG_C_TASK_CONTROL_H
#define FOUG_C_TASK_CONTROL_H

#include "global.h"
#include "memory.h"

/*! Provides control over a general task.
 *
 *  "Control" here means task progress handling and interruption request (abort).
 */
struct foug_task_control
{
  /*! Opaque pointer on a user task object, passed as first argument to hook functions */
  void* cookie;

  /*! Pointer on a function that that does something with progress value \p progress (eg update
   *  a UI progress bar)
   *
   * The return value is important : it is used as an interruption request status. If FOUG_TRUE then
   * the current task can continue, otherwise it should abort as soon as possible.
   */
  foug_bool_t (*handle_progress_func)(void* cookie, uint8_t progress);
};

/*! Convenient typedef for struct foug_task_control */
typedef struct foug_task_control foug_task_control_t;

/*! Safe and convenient function for foug_task_control::handle_progress_func()
 *
 *  Basically the same as : \code ctrl->handle_progress_func(ctrl->cookie, progress_pc) \endcode
 */
FOUG_LIB_EXPORT foug_bool_t foug_task_control_handle_progress(foug_task_control_t* ctrl,
                                                              uint8_t progress_pc);

/*! Utility function that converts \p value as a percentage
 *
 * \p value is assumed to be relative to the interval (range) defined by
 * [ \p range_min , \p range_max ]
 */
FOUG_LIB_EXPORT uint8_t foug_percentage(size_t range_min, size_t range_max, size_t value);

#endif /* FOUG_C_TASK_CONTROL_H */
