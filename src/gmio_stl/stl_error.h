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

/*! \file stl_error.h
 *  List of errors specific to STL I/O functions
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#pragma once

namespace gmio {

//! Byte-mask to tag(identify) STL-specific error codes
const unsigned STL_TagError = 0x01000000;

//! Specific error codes reported by STL read/write functions
enum STL_Error {
    //! STL format could not be guessed in read function
    STL_Error_UnknownFormat = STL_TagError + 0x01,

    // Specific error codes returned by STL_ascii read function

    //! Parsing error occured due to malformed STL ascii input
    STL_Error_Parsing,

    //! Invalid max number of decimal significants digits must be in [1..9]
    STL_Error_InvalidFloat32Prec,

    // Specific error codes returned by STL_binary read/write functions

    //! The byte order argument supplied is not supported, must be little or
    //! big endian
    STL_Error_UnsupportedByteOrder,

    //! Error occured when reading header data in STL_readBinary()
    STL_Error_HeaderWrongSize,

    //! Error occured when reading facet count in STL_readBinary()
    STL_Error_FacetCount
};

} // namespace gmio

/*! @} */
