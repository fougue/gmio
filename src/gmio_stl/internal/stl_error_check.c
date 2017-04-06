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

#include "stl_error_check.h"

#include "../../gmio_core/error.h"
#include "../stl_error.h"
#include "../stl_io.h"

bool gmio_stl_check_mesh(int *error, const struct gmio_stl_mesh* mesh)
{
    if (mesh == NULL
            || (mesh->triangle_count > 0 && mesh->func_get_triangle == NULL))
    {
        *error = GMIO_STL_ERROR_NULL_FUNC_GET_TRIANGLE;
    }
    return gmio_no_error(*error);
}

bool gmio_stlb_check_byteorder(int* error, enum gmio_endianness byte_order)
{
    if (byte_order != GMIO_ENDIANNESS_LITTLE
            && byte_order != GMIO_ENDIANNESS_BIG)
    {
        *error = GMIO_STL_ERROR_UNSUPPORTED_BYTE_ORDER;
    }
    return gmio_no_error(*error);
}

bool gmio_stla_check_float32_precision(int *error, uint8_t prec)
{
    if (prec == 0 || prec > 9)
        *error = GMIO_STL_ERROR_INVALID_FLOAT32_PREC;
    return gmio_no_error(*error);
}
