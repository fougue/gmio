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

/*! \file stlb_header.h
 *  Declaration of gmio_stlb_header
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#pragma once

#include "stl_global.h"
#include "stl_constants.h"

#include "../gmio_core/span.h"

#include <array>

namespace gmio {

//! 80-byte data at the beginning of any STL binary file
typedef std::array<uint8_t, STL_BinaryHeaderSize> STL_BinaryHeader;

//! Copies 'header' into C string 'str'
//!
//! It replaces non-printable bytes with 'replacement' char.
//!
//! 'str' must be at least \c STL_BinaryHeaderSize+1 long, a terminating
//! null character ('\0') is copied at position \c STL_BinaryHeaderSize
GMIO_API std::array<char, STL_BinaryHeaderSize+1> STL_binaryHeaderToString(
        const STL_BinaryHeader& header, char replacement = ' ');

//! Returns a BinaryHeader object whose contents is a copy of 'str'
//!
//! Only the first \c STL_BinaryHeaderSize characters are copied.
//!
//! If the length of 'str' is less than \c STL_BinaryHeaderSize then the
//! remaining bytes are filled with zeroes.
GMIO_API STL_BinaryHeader STL_binaryHeaderFromString(Span<const char> str);

} // namespace gmio

/*! @} */
