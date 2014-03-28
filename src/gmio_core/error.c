#include "error.h"

gmio_bool_t gmio_no_error(int code)
{
  return code == GMIO_NO_ERROR;
}

gmio_bool_t gmio_error(int code)
{
  return code != GMIO_NO_ERROR;
}
