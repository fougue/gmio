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

#ifndef GMIO_INTERNAL_HELPER_TASK_IFACE_H
#define GMIO_INTERNAL_HELPER_TASK_IFACE_H

#include "../task_iface.h"

#include <stddef.h>

/*! Safe and convenient function for gmio_task_iface::func_is_stop_requested() */
GMIO_INLINE gmio_bool_t gmio_task_iface_is_stop_requested(
        const struct gmio_task_iface* itask)
{
    if (itask != NULL && itask->func_is_stop_requested != NULL)
        return itask->func_is_stop_requested(itask->cookie);
    return GMIO_FALSE;
}

/*! Safe and convenient function for gmio_task_iface::func_handle_progress() */
GMIO_INLINE void gmio_task_iface_handle_progress(
        const struct gmio_task_iface* itask, size_t value, size_t max_value)
{
    if (itask != NULL && itask->func_handle_progress != NULL)
        itask->func_handle_progress(itask->cookie, value, max_value);
}

#endif /* GMIO_INTERNAL_HELPER_TASK_IFACE_H */
