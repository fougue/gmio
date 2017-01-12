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

/*! \file stl_error.h
 *  List of errors specific to STL I/O functions
 *
 *  \addtogroup gmio_stl
 *  @{
 */

#ifndef GMIO_STL_ERROR_H
#define GMIO_STL_ERROR_H

/*! \c GMIO_STL_ERROR_TAG
 *  Byte-mask to tag(identify) STL-specific error codes */
enum { GMIO_STL_ERROR_TAG = 0x01000000 };

/*! Specific error codes reported by STL read/write functions */
enum gmio_stl_error
{
    /*! STL format could not be guessed in read function */
    GMIO_STL_ERROR_UNKNOWN_FORMAT = GMIO_STL_ERROR_TAG + 0x01,

    /*! Common STL write error indicating gmio_stl_mesh::func_get_triangle()
     *  pointer is NULL */
    GMIO_STL_ERROR_NULL_FUNC_GET_TRIANGLE = GMIO_STL_ERROR_TAG + 0x02,

    /* Specific error codes returned by STL_ascii read function */

    /*! Parsing error occured due to malformed STL ascii input */
    GMIO_STL_ERROR_PARSING = GMIO_STL_ERROR_TAG + 0x100,

    /*! Invalid max number of decimal significants digits must be in [1..9] */
    GMIO_STL_ERROR_INVALID_FLOAT32_PREC = GMIO_STL_ERROR_TAG + 0x101,

    /* Specific error codes returned by STL_binary read/write functions */

    /*! The byte order argument supplied is not supported, must be little or
     *  big endian */
    GMIO_STL_ERROR_UNSUPPORTED_BYTE_ORDER = GMIO_STL_ERROR_TAG + 0x1000,

    /*! Error occured when reading header data in gmio_stlb_read() */
    GMIO_STL_ERROR_HEADER_WRONG_SIZE = GMIO_STL_ERROR_TAG + 0x1001,

    /*! Error occured when reading facet count in gmio_stlb_read() */
    GMIO_STL_ERROR_FACET_COUNT = GMIO_STL_ERROR_TAG + 0x1002
};

#endif /* GMIO_STL_ERROR_H */
/*! @} */
