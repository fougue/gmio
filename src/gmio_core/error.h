/****************************************************************************
** GeomIO Library
** Copyright FougSys (2 Mar. 2015)
** contact@fougsys.fr
**
** This software is a reusable library whose purpose is to provide complete
** I/O support for various CAD file formats (eg. STL)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info".
****************************************************************************/

/*! \file error.h
 *  List of common errors, reported by I/O functions
 */

#ifndef GMIO_ERROR_H
#define GMIO_ERROR_H

#include "global.h"

GMIO_C_LINKAGE_BEGIN

/*! This enum defines common errors */
enum gmio_error
{
    /*! No error occurred, success */
    GMIO_NO_ERROR = 0,

    /*! Pointer on argument gmio_transfer_t is NULL */
    GMIO_NULL_TRANSFER_ERROR = -1,

    /*! Pointer on argument buffer is NULL */
    GMIO_NULL_BUFFER_ERROR = -2,

    /*! Argument buffer's size is too small */
    GMIO_INVALID_BUFFER_SIZE_ERROR = -3,

    /*! An error occurred with the argument gmio_stream_t */
    GMIO_STREAM_ERROR = -4,

    /*! Operation was stopped by user, that is to say
     *  gmio_task_control::is_stop_requested_func() returned GMIO_FALSE */
    GMIO_TASK_STOPPED_ERROR = -5
};

typedef enum gmio_error gmio_error_t;

/*! Returns true if \p code == GMIO_NO_ERROR */
GMIO_LIB_EXPORT gmio_bool_t gmio_no_error(int code);

/*! Returns true if \p code != GMIO_NO_ERROR */
GMIO_LIB_EXPORT gmio_bool_t gmio_error(int code);

GMIO_C_LINKAGE_END

#endif /* GMIO_ERROR_H */
