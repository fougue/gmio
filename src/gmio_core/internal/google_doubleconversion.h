/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#ifndef GMIO_INTERNAL_GOOGLE_DOUBLECONVERSION_H
#define GMIO_INTERNAL_GOOGLE_DOUBLECONVERSION_H

#include "../global.h"
#include "../text_format.h"

#include <stddef.h>

/* Google double-conversion
 *     https://github.com/google/double-conversion
 *     commit #56a0457
 */

GMIO_C_LINKAGE_BEGIN
float gmio_str2float_googledoubleconversion(const char* num, size_t numlen);
int gmio_float2str_googledoubleconversion(
        float value,
        char* buff,
        size_t bufflen,
        enum gmio_float_text_format textformat,
        uint8_t prec);
GMIO_C_LINKAGE_END

#endif /* GMIO_INTERNAL_GOOGLE_DOUBLECONVERSION_H */
