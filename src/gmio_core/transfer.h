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

/*! \file transfer.h
 *  Declaration of gmio_transfer
 *
 *  \addtogroup gmio_core
 *  @{
 */

#ifndef GMIO_TRANSFER_H
#define GMIO_TRANSFER_H

#include "global.h"
#include "memblock.h"
#include "stream.h"
#include "task_iface.h"

/*! Defines objects required for any transfer(read/write) operation */
struct gmio_transfer
{
    /*! The stream object to be used for I/O */
    gmio_stream_t stream;

    /*! The memory block used by the transfer for stream buffering */
    gmio_memblock_t memblock;

    /*! The interface object by which the transfer task can be controlled */
    gmio_task_iface_t task_iface;
};

typedef struct gmio_transfer gmio_transfer_t;

#endif /* GMIO_TRANSFER_H */
/*! @} */
