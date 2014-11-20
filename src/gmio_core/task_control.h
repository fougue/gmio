#ifndef GMIO_TASK_CONTROL_H
#define GMIO_TASK_CONTROL_H

#include "global.h"

GMIO_C_LINKAGE_BEGIN

/*! Provides control over a general task.
 *
 *  "Control" here means task interruption request (abort).
 */
struct gmio_task_control
{
  /*! Opaque pointer on a user task object, passed as first argument to hook functions */
  void* cookie;

  /*! \brief Pointer on a function that says if the current task must stop
   *
   *  If GMIO_TRUE is returned then the current task should abort as soon as possible, otherwise it
   *  can continue execution.
   */
  gmio_bool_t (*is_stop_requested_func)(void* cookie);
};

typedef struct gmio_task_control gmio_task_control_t;

GMIO_LIB_EXPORT
gmio_bool_t gmio_task_control_is_stop_requested(const gmio_task_control_t* task_ctrl);

GMIO_C_LINKAGE_END

#endif /* GMIO_TASK_CONTROL_H */
