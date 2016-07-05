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

/*! \file stlb_header.h
 *  Declaration of gmio_stlb_header
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STLB_HEADER_H
#define GMIO_STLB_HEADER_H

#include "stl_global.h"
#include "stl_constants.h"

/*! 80-byte data at the beginning of any STL binary file */
struct gmio_stlb_header
{
    uint8_t data[GMIO_STLB_HEADER_SIZE];
};

GMIO_C_LINKAGE_BEGIN

/*! Returns a gmio_stlb_header object whose contents is a copy of \p str
 *
 *  Only the first \c GMIO_STLB_HEADER_SIZE characters are copied.
 *
 *  If the length of \p str is less than \c GMIO_STLB_HEADER_SIZE then the
 *  remaining bytes are filled with zeroes.
 */
GMIO_API struct gmio_stlb_header gmio_stlb_header_str(const char* str);

/*! Copies \p header into C string \p str
 *
 *  It replaces non-printable bytes with \p replacement char.
 *
 *  \p str must be at least \c GMIO_STLB_HEADER_SIZE+1 long, a terminating null
 *  character ('\0') is copied at position \c GMIO_STLB_HEADER_SIZE
 */
GMIO_API void gmio_stlb_header_to_printable_str(
                const struct gmio_stlb_header* header,
                char* str,
                char replacement);

GMIO_C_LINKAGE_END

#endif /* GMIO_STLB_HEADER_H */
/*! @} */
