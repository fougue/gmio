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

/*! \file rwargs.h
 *  Declares structures for the common arguments of read/write operations
 *
 *  \addtogroup gmio_core
 *  @{
 */

#ifndef GMIO_RWARGS_H
#define GMIO_RWARGS_H

#include "stream.h"
#include "memblock.h"
#include "task_iface.h"

/*! Common arguments for read or write functions */
struct gmio_rwargs
{
    /*! The stream object to be used for I/O */
    struct gmio_stream stream;

    /*! Optional memory block used by the stream to bufferize I/O device
     *  operations
     *
     *  If passed null to a read/write function, then a temporary memblock
     *  is created with the global default constructor function (see
     *  gmio_memblock_default())
     */
    struct gmio_memblock stream_memblock;

    /*! Optional interface by which the I/O operation can be controlled */
    struct gmio_task_iface task_iface;
};

#endif /* GMIO_RWARGS_H */
