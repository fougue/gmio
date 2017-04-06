/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

bool gmio_vec3f_equal(
        const struct gmio_vec3f *lhs,
        const struct gmio_vec3f *rhs,
        uint32_t max_ulp_diff)
{
    return gmio_float32_ulp_equals(lhs->x, rhs->x, max_ulp_diff)
            && gmio_float32_ulp_equals(lhs->y, rhs->y, max_ulp_diff)
            && gmio_float32_ulp_equals(lhs->z, rhs->z, max_ulp_diff);
}
