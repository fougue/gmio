#include "stl_rw_common.h"

#include "../../datax_core/error.h"
#include "../stl_error.h"

foug_bool_t foug_check_transfer(int *error, const foug_transfer_t* trsf)
{
  if (trsf == NULL) {
    *error = FOUG_DATAX_NULL_TRANSFER_ERROR;
  }
  else {
    if (trsf->buffer == NULL)
      *error = FOUG_DATAX_NULL_BUFFER_ERROR;
    else if (trsf->buffer_size == 0)
      *error = FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;
  }

  return foug_datax_no_error(*error);
}

foug_bool_t foug_stl_check_geom(int *error, const foug_stl_geom_t* geom)
{
  if (geom == NULL
      || (geom->triangle_count > 0 && geom->get_triangle_func == NULL))
  {
    *error = FOUG_STL_WRITE_NULL_GET_TRIANGLE_FUNC_ERROR;
  }

  return foug_datax_no_error(*error);
}

foug_bool_t foug_stlb_check_params(int *error,
                                   const foug_transfer_t *trsf,
                                   foug_endianness_t byte_order)
{
  if (!foug_check_transfer(error, trsf))
    return FOUG_FALSE;

  if (trsf->buffer_size < FOUG_STLB_MIN_CONTENTS_SIZE)
    *error = FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;
  if (byte_order != FOUG_LITTLE_ENDIAN && byte_order != FOUG_BIG_ENDIAN)
    *error = FOUG_STLB_UNSUPPORTED_BYTE_ORDER_ERROR;

  return foug_datax_no_error(*error);
}
