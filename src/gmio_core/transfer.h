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

#ifndef GMIO_TRANSFER_H
#define GMIO_TRANSFER_H

#include "global.h"
#include "stream.h"
#include "task_control.h"

/*! Defines objects required for any transfer(read/write) operation */
struct gmio_transfer
{
    /*! The stream object to be used for I/O */
    gmio_stream_t       stream;

    /*! The optional object used to control execution of the transfer */
    gmio_task_control_t task_control;

    /*! Pointer on a memory buffer used by the transfer for stream operations */
    void*               buffer;

    /*! Size (in bytes) of the memory buffer */
    size_t              buffer_size;
};

typedef struct gmio_transfer gmio_transfer_t;

#endif /* GMIO_TRANSFER_H */
