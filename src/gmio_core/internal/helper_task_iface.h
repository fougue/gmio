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
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#ifndef GMIO_INTERNAL_HELPER_TASK_IFACE_H
#define GMIO_INTERNAL_HELPER_TASK_IFACE_H

#include "../task_iface.h"

#include <stddef.h>

/*! Safe and convenient function for gmio_task_iface::is_stop_requested_func() */
GMIO_INLINE gmio_bool_t gmio_task_iface_is_stop_requested(
        const gmio_task_iface_t* itask)
{
    if (itask != NULL && itask->is_stop_requested_func != NULL)
        return itask->is_stop_requested_func(itask->cookie);
    return GMIO_FALSE;
}

/*! Safe and convenient function for gmio_task_iface::handle_progress_func() */
GMIO_INLINE void gmio_task_iface_handle_progress(
        const gmio_task_iface_t* itask, size_t value, size_t max_value)
{
    if (itask != NULL && itask->handle_progress_func != NULL)
        itask->handle_progress_func(itask->cookie, value, max_value);
}

#endif /* GMIO_INTERNAL_HELPER_TASK_IFACE_H */
