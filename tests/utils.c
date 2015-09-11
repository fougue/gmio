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

#include "utils.h"

gmio_bool_t gmio_float32_equals_by_ulp(
        gmio_float32_t a, gmio_float32_t b, uint32_t max_ulp_diff)
{
    const int32_t ia = gmio_convert_int32(a);
    const int32_t ib = gmio_convert_int32(b);
    const int32_t slp_diff = ia - ib;
    const uint32_t ulp_diff = slp_diff > 0 ? slp_diff : -slp_diff;

    /* Different signs, we could maybe get difference to 0, but so close to 0
     * using epsilons is better */
    if (gmio_int32_sign(ia) != gmio_int32_sign(ib)) {
        /* Check for equality to make sure +0 == -0 */
        return ia == ib;
    }

    /* Find the difference in ULPs */
    return ulp_diff <= max_ulp_diff;
}

uint32_t gmio_float32_ulp_diff(gmio_float32_t a, gmio_float32_t b)
{
    const uint32_t ua = gmio_convert_uint32(a);
    const uint32_t ub = gmio_convert_uint32(b);
    return ua > ub ? ua - ub : ub - ua;
}

void gmio_string_trim_from_end(char *str, size_t len)
{
    if (len > 0) {
        do {
            --len;
            if (str[len] == 0 || gmio_clocale_isspace(str[len]))
                str[len] = 0;
            else
                break;
        } while (len != 0);
    }
}
