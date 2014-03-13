#include "error.h"

foug_bool_t foug_datax_no_error(int code)
{
  return code == FOUG_DATAX_NO_ERROR;
}

foug_bool_t foug_datax_error(int code)
{
  return code != FOUG_DATAX_NO_ERROR;
}
