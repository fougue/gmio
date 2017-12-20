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

/*! \file stl_format.h
 *  Detection of STL format from input stream
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#pragma once

#include "stl_global.h"
#include "../gmio_core/span.h"

namespace gmio {

//! Byte-mask to tag(identify) STL binary formats
static const unsigned STL_Format_TagBinary = 0x10;

//! Various STL formats
enum STL_Format {
    STL_Format_Unknown = 0,
    STL_Format_Ascii = 0x01,
    STL_Format_BinaryLittleEndian = STL_Format_TagBinary | 0x01,
    STL_Format_BinaryBigEndian = STL_Format_TagBinary | 0x02
};

//! Returns the format of the STL data from input bytes
//!
//! \retval STL_Format_Unknwon in case of error.
GMIO_API STL_Format STL_probeFormat(
        Span<const uint8_t> bytes, uint64_t hint_solid_size = 0);

//! Returns the format of the STL data in file at location 'filepath'
//!
//! This function is a wrapper around STL_probeFormat(Span<const uint8_t>, uint64_t)
//!
//! 'filepath' is the path to the STL file. A stream is opened with \c fopen()
//!  so the string shall follow the file name specifications of the running
//!  environment
GMIO_API STL_Format STL_probeFormat(const char* filepath);

} // namespace gmioStl

/*! @} */
