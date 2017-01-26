/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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

#include "const_string.h"
#include "min_max.h"

#include <string.h>

size_t gmio_const_string_concat(
        char* dst, size_t dst_capacity,
        const struct gmio_const_string* lhs,
        const struct gmio_const_string* rhs)
{
    const size_t capacity_sub_rhs = dst_capacity - rhs->len - 1;
    const size_t lhs_copy_len = GMIO_MIN(capacity_sub_rhs, lhs->len);
    strncpy(dst, lhs->ptr, lhs_copy_len);
    const size_t dst_remaining_cap = dst_capacity - lhs_copy_len;
    const size_t rhs_copy_len = GMIO_MIN(dst_remaining_cap, rhs->len);
    strncpy(dst + lhs_copy_len, rhs->ptr, rhs_copy_len);
    dst[lhs_copy_len + rhs_copy_len] = '\0';
    return lhs_copy_len + rhs_copy_len;
}
