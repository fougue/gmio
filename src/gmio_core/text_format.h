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

/*! \file text_format.h
 *  Formats for textual representation
 *
 *  \addtogroup gmio_core
 *  @{
 */

#ifndef GMIO_TEXT_FORMAT_H
#define GMIO_TEXT_FORMAT_H

#include "global.h"

/*! Various formats to textually represent a float(single/double precision) */
enum gmio_float_text_format
{
    /*! Decimal floating point, lowercase (ex: 392.65) */
    GMIO_FLOAT_TEXT_FORMAT_DECIMAL_LOWERCASE = 0,
    /*! Decimal floating point, uppercase (ex: 392.65) */
    GMIO_FLOAT_TEXT_FORMAT_DECIMAL_UPPERCASE,
    /*! Scientific notation, lowercase (ex: 3.9265e+2) */
    GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_LOWERCASE,
    /*! Scientific notation, uppercase (ex: 3.9265E+2) */
    GMIO_FLOAT_TEXT_FORMAT_SCIENTIFIC_UPPERCASE,
    /*! Use the shortest representation: decimal or scientific lowercase */
    GMIO_FLOAT_TEXT_FORMAT_SHORTEST_LOWERCASE,
    /*! Use the shortest representation: decimal or scientific uppercase */
    GMIO_FLOAT_TEXT_FORMAT_SHORTEST_UPPERCASE
};

#endif /* GMIO_TEXT_FORMAT_H */
/*! @} */
