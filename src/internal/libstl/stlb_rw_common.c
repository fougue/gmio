#include "stlb_rw_common.h"

#include "../../error.h"
#include "../../libstl/stl_error.h"

int foug_stlb_check_params(const foug_transfer_t *trsf,
                           foug_endianness_t byte_order)
{
  if (trsf == NULL)
    return FOUG_DATAX_NULL_TRANSFER_ERROR;
  if (trsf->buffer == NULL)
    return FOUG_DATAX_NULL_BUFFER_ERROR;
  if (trsf->buffer_size < FOUG_STLB_MIN_CONTENTS_SIZE)
    return FOUG_DATAX_INVALID_BUFFER_SIZE_ERROR;
  if (byte_order != FOUG_LITTLE_ENDIAN && byte_order != FOUG_BIG_ENDIAN)
    return FOUG_STLB_READWRITE_UNSUPPORTED_BYTE_ORDER;

  return FOUG_DATAX_NO_ERROR;
}
