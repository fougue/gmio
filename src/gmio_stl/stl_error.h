/****************************************************************************
** gmio
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

/*! \file stl_error.h
 *  List of errors specific to STL I/O functions
 */

#ifndef GMIO_STL_ERROR_H
#define GMIO_STL_ERROR_H

/*! A byte-mask to tag(identify) STL-specific error codes */
enum { GMIO_STL_ERROR_TAG = 0x11000000 };

/*! This enum defines the various error codes reported by STL read/write
 *  functions */
enum gmio_stl_error
{
    /*! STL format could not be guessed in read function */
    GMIO_STL_ERROR_UNKNOWN_FORMAT = GMIO_STL_ERROR_TAG + 0x01,

    /*! Common STL write error indicating gmio_stl_mesh::func_get_triangle()
     *  pointer is NULL */
    GMIO_STL_ERROR_NULL_GET_TRIANGLE_FUNC = GMIO_STL_ERROR_TAG + 0x02,

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

typedef enum gmio_stl_error  gmio_stl_error_t;

#endif /* GMIO_STL_ERROR_H */
