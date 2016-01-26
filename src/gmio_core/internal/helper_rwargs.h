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

#ifndef GMIO_INTERNAL_HELPER_RWARGS_H
#define GMIO_INTERNAL_HELPER_RWARGS_H

#include "../rwargs.h"
#include "helper_task_iface.h"

/*! Safe and convenient function for gmio_task_iface::func_is_stop_requested()
 *  through gmio_transfer::task_iface
 */
GMIO_INLINE bool gmio_rwargs_is_stop_requested(
        const struct gmio_rwargs* args)
{
    if (args != NULL)
        return gmio_task_iface_is_stop_requested(&args->task_iface);
    return false;
}

/*! Safe and convenient function for gmio_task_iface::func_handle_progress()
 *  through gmio_transfer::task_iface
 */
GMIO_INLINE void gmio_rwargs_handle_progress(
        const struct gmio_rwargs* args, size_t value, size_t max_value)
{
    if (args != NULL)
        gmio_task_iface_handle_progress(&args->task_iface, value, max_value);
}

#endif /* GMIO_INTERNAL_HELPER_RWARGS_H */
