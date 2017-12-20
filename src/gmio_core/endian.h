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

/*! \file endian.h
 *  Architecture endianness
 *
 *  \addtogroup gmio_core
 *  @{
 */

#pragma once

#include "global.h"

namespace gmio {

//! Common endianness (byte order) of computer memory
enum Endianness {
    //! Other (unknown) byte-order
    Endianness_Unknown = 0,

    //! The least significant byte is stored at the lowest address. The other
    //! bytes follow in increasing order of significance
    Endianness_Little,

    //! The most significant byte is stored at the lowest address. The other
    //! bytes follow in decreasing order of significance
    Endianness_Big,

#ifdef GMIO_HOST_IS_BIG_ENDIAN
    Endianness_Host = Endianness_Big
#else
    //! Endianness(byte order) used by the host computer for storing data in
    //! memory.
    //!
    //! Set at configure-time to either Little or Big
    Endianness_Host = Endianness_Little
#endif
};

//! Returns endianness (byte order) of the host's CPU architecture
GMIO_API Endianness hostEndianness();

} // namespace gmio

/*! @} */
