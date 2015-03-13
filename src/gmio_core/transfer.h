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

/*! \file transfer.h
 *  Declaration of gmio_transfer
 */

#ifndef GMIO_TRANSFER_H
#define GMIO_TRANSFER_H

#include "global.h"
#include "stream.h"

GMIO_C_LINKAGE_BEGIN

/*! Defines objects required for any transfer(read/write) operation */
struct gmio_transfer
{
    /*! Optional opaque pointer on a user task object, passed as first
     *  argument to hook functions */
    void* cookie;

    /*! Optional pointer on a function that says if the currently running
     *  operation must stop
     *
     *  If \c GMIO_TRUE is returned then the current transfer should abort as
     *  soon as possible, otherwise it can continue execution.
     */
    gmio_bool_t (*is_stop_requested_func)(void* cookie);

    /*! Optional pointer on a function that is called anytime some progress
     *  was done during transfer
     *
     *  \param cookie The cookie set inside the gmio_transfer object
     *  \param value Current value of the transfer progress (<= \p max_value )
     *  \param max_value Maximum value of the transfer progress
     */
    void (*handle_progress_func)(void* cookie, size_t value, size_t max_value);

    /*! The stream object to be used for I/O */
    gmio_stream_t stream;

    /*! Pointer on a memory buffer used by the transfer for stream
     *  operations */
    void* buffer;

    /*! Size (in bytes) of the memory buffer */
    size_t buffer_size;
};

typedef struct gmio_transfer gmio_transfer_t;

/*! Safe and convenient function for gmio_transfer::is_stop_requested_func()
 *
 *  Same as: \code trsf->is_stop_requested_func(trsf->cookie) \endcode
 *
 *  TODO: don't export, move to gmio_core/internal
 */
GMIO_LIB_EXPORT
gmio_bool_t gmio_transfer_is_stop_requested(const gmio_transfer_t* trsf);

/*! Safe and convenient function for gmio_transfer::handle_progress_func()
 *
 *  Same as: \code trsf->handle_progress_func(trsf->cookie, v, maxv) \endcode
 *
 *  TODO: don't export, move to gmio_core/internal
 */
GMIO_LIB_EXPORT
gmio_bool_t gmio_transfer_handle_progress(
        const gmio_transfer_t* trsf, size_t value, size_t max_value);

GMIO_C_LINKAGE_END

#endif /* GMIO_TRANSFER_H */
