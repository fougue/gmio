#ifndef FOUG_DATAX_C_ERROR_H
#define FOUG_DATAX_C_ERROR_H

#include "global.h"

/* Common errors */
typedef enum
{
  FOUG_DATAX_NO_ERROR = 0,
  FOUG_DATAX_NULL_TRANSFER_ERROR = -1,
  FOUG_DATAX_NULL_BUFFER_ERROR = -2,
  FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR = -3,
  FOUG_DATAX_STREAM_ERROR = -4,
  FOUG_DATAX_TASK_STOPPED_ERROR = -5
} foug_datax_error_t;

FOUG_LIB_EXPORT foug_bool_t foug_datax_no_error(int code);
FOUG_LIB_EXPORT foug_bool_t foug_datax_error(int code);

#endif /* FOUG_DATAX_C_ERROR_H */
