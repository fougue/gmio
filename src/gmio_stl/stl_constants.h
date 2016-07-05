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

/*! \file stl_constants.h
 *  STL constants
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_CONSTANTS_H
#define GMIO_STL_CONSTANTS_H

#include "../gmio_core/global.h"

enum gmio_stl_constants
{
    /*! Compact size of a gmio_vec3f object */
    GMIO_STL_COORDS_RAWSIZE = (3 * sizeof(float)),

    /*! Compact size of a gmio_stl_triangle object, STL ascii format */
    GMIO_STLA_TRIANGLE_RAWSIZE = (4 * GMIO_STL_COORDS_RAWSIZE),

    /*! Compact size of a gmio_stl_triangle object, STL binary format */
    GMIO_STLB_TRIANGLE_RAWSIZE = (GMIO_STLA_TRIANGLE_RAWSIZE + sizeof(uint16_t)),

    /*! Size(in bytes) of the header data for STL binary */
    GMIO_STLB_HEADER_SIZE = 80,

    /*! Size(in bytes) of the minimum contents possible with the STL binary
     *  format */
    GMIO_STLB_MIN_CONTENTS_SIZE = 284
};

#endif /* GMIO_STL_CONSTANTS_H */
/*! @} */
