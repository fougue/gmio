/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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

/*! \file error.h
 *  List of common errors, reported by I/O functions
 *
 *  \addtogroup gmio_core
 *  @{
 */

#ifndef GMIO_ERROR_H
#define GMIO_ERROR_H

#include "global.h"

/*! This enum defines common errors */
enum gmio_error
{
    /*! No error occurred, success */
    GMIO_ERROR_OK = 0,

    /*! Pointer on argument memory block is NULL */
    GMIO_ERROR_NULL_MEMBLOCK,

    /*! Argument size for the memory block is too small */
    GMIO_ERROR_INVALID_MEMBLOCK_SIZE,

    /*! An error occurred with gmio_stream */
    GMIO_ERROR_STREAM,

    /*! Transfer was stopped by user, that is to say
     *  gmio_transfer::func_is_stop_requested() returned true */
    GMIO_ERROR_TRANSFER_STOPPED,

    /*! An error occured after a call to a <stdio.h> function
     *
     *  The caller can check errno to get the real error number
     */
    GMIO_ERROR_STDIO,

    /*! Unknown error */
    GMIO_ERROR_UNKNOWN
};

/*! Returns true if <tt>code == GMIO_NO_ERROR</tt> */
GMIO_INLINE bool gmio_no_error(int code)
{ return code == GMIO_ERROR_OK ? true : false; }

/*! Returns true if <tt>code != GMIO_NO_ERROR</tt> */
GMIO_INLINE bool gmio_error(int code)
{ return code != GMIO_ERROR_OK ? true : false; }

#endif /* GMIO_ERROR_H */
/*! @} */
