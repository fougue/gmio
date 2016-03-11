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

#include "core_utils.h"

#include "../src/gmio_core/internal/numeric_utils.h"
#include "../src/gmio_core/internal/string_ascii_utils.h"

void gmio_string_trim_from_end(char *str, size_t len)
{
    if (len > 0) {
        do {
            --len;
            if (str[len] == 0 || gmio_ascii_isspace(str[len]))
                str[len] = 0;
            else
                break;
        } while (len != 0);
    }
}

bool gmio_vec3_f32_equal(
        const struct gmio_vec3_f32 *lhs,
        const struct gmio_vec3_f32 *rhs,
        uint32_t max_ulp_diff)
{
    return gmio_float32_ulp_equals(lhs->x, rhs->x, max_ulp_diff)
            && gmio_float32_ulp_equals(lhs->y, rhs->y, max_ulp_diff)
            && gmio_float32_ulp_equals(lhs->z, rhs->z, max_ulp_diff);
}
