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

/*! \file task_iface.h
 *  Declaration of gmio_task_iface
 */

#ifndef GMIO_TASK_IFACE_H
#define GMIO_TASK_IFACE_H

#include "global.h"

/*! Defines an interface through which any task can be controlled */
struct gmio_task_iface
{
    /*! Optional opaque pointer on a user task object, passed as first
     *  argument to hook functions */
    void* cookie;

    /*! Optional pointer on a function that says if the currently running
     *  task must stop
     *
     *  If \c GMIO_TRUE is returned then the current task should abort as
     *  soon as possible, otherwise it can continue execution.
     */
    gmio_bool_t (*func_is_stop_requested)(void* cookie);

    /*! Optional pointer on a function that is called anytime some new progress
     *  was done
     *
     *  \param cookie The cookie set inside the gmio_task_iface object
     *  \param value Current value of the task progress (<= \p max_value )
     *  \param max_value Maximum value of the task progress
     */
    void (*func_handle_progress)(void* cookie, size_t value, size_t max_value);
};

typedef struct gmio_task_iface gmio_task_iface_t;

#endif /* GMIO_TASK_IFACE_H */
