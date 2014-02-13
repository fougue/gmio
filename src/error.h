#ifndef FOUG_DATAX_C_ERROR_H
#define FOUG_DATAX_C_ERROR_H

#include "global.h"

/*! This enum defines common errors */
enum foug_datax_error
{
  /*! No error occurred, success */
  FOUG_DATAX_NO_ERROR = 0,

  /*! Pointer on argument foug_transfer_t is NULL */
  FOUG_DATAX_NULL_TRANSFER_ERROR = -1,

  /*! Pointer on argument buffer is NULL */
  FOUG_DATAX_NULL_BUFFER_ERROR = -2,

  /*! Argument buffer's size is too small */
  FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR = -3,

  /*! An error occurred with the argument foug_stream_t (check foug_stream_error()) */
  FOUG_DATAX_STREAM_ERROR = -4,

  /*! Operation was stopped by user (foug_task_control::handle_progress_func() returned FOUG_FALSE) */
  FOUG_DATAX_TASK_STOPPED_ERROR = -5
};

typedef enum foug_datax_error foug_datax_error_t;

/*! Returns true if \p code == FOUG_DATAX_NO_ERROR */
FOUG_LIB_EXPORT foug_bool_t foug_datax_no_error(int code);

/*! Returns true if \p code != FOUG_DATAX_NO_ERROR */
FOUG_LIB_EXPORT foug_bool_t foug_datax_error(int code);

#endif /* FOUG_DATAX_C_ERROR_H */
