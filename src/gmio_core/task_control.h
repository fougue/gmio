#ifndef GMIO_TASK_CONTROL_H
#define GMIO_TASK_CONTROL_H

#include "global.h"
#include "memory.h"

GMIO_C_LINKAGE_BEGIN

/*! Provides control over a general task.
 *
 *  "Control" here means task progress handling and interruption request (abort).
 */
struct gmio_task_control
{
  /*! Opaque pointer on a user task object, passed as first argument to hook functions */
  void* cookie;

  /*! Pointer on a function that that does something with progress value \p progress */
  gmio_bool_t (*handle_progress_func)(void* cookie, uint8_t progress);
};

typedef struct gmio_task_control gmio_task_control_t;

/*! Safe and convenient function for gmio_task_control::handle_progress_func() */
GMIO_LIB_EXPORT gmio_bool_t gmio_task_control_handle_progress(gmio_task_control_t* ctrl,
                                                              uint8_t progress_pc);

/*! Utility function that converts \p value as a percentage */
GMIO_LIB_EXPORT uint8_t gmio_percentage(size_t range_min, size_t range_max, size_t value);

GMIO_C_LINKAGE_END

#endif /* GMIO_TASK_CONTROL_H */
