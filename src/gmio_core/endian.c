/****************************************************************************
**
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
**
****************************************************************************/

#include "endian.h"

#include "internal/convert.h"

#include <string.h>

typedef union
{
  uint32_t integer;
  uint8_t  bytes[4];
} _internal_gmio_int_bytes_32_convert_t;

gmio_endianness_t gmio_host_endianness()
{
  _internal_gmio_int_bytes_32_convert_t conv;

  conv.integer = 0x01020408;
  if (conv.bytes[0] == 0x08 && conv.bytes[3] == 0x01)
    return GMIO_LITTLE_ENDIAN;
  else if (conv.bytes[0] == 0x01 && conv.bytes[3] == 0x08)
    return GMIO_BIG_ENDIAN;
  else if (conv.bytes[1] == 0x08 && conv.bytes[2] == 0x01)
    return GMIO_MIDDLE_ENDIAN;
  else
    return GMIO_OTHER_ENDIAN;
}

