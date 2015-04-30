/****************************************************************************
** GeomIO Library
** Copyright Fougue (2 Mar. 2015)
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

#ifndef GMIO_INTERNAL_HELPER_TRANSFER_H
#define GMIO_INTERNAL_HELPER_TRANSFER_H

#include "../transfer.h"
#include "helper_task_iface.h"

/*! Safe and convenient function for gmio_task_iface::is_stop_requested_func()
 *  through gmio_transfer::task_iface
 */
GMIO_INLINE gmio_bool_t gmio_transfer_is_stop_requested(
        const gmio_transfer_t* trsf)
{
    if (trsf != NULL)
        return gmio_task_iface_is_stop_requested(&trsf->task_iface);
    return GMIO_FALSE;
}

/*! Safe and convenient function for gmio_task_iface::handle_progress_func()
 *  through gmio_transfer::task_iface
 */
GMIO_INLINE void gmio_transfer_handle_progress(
        const gmio_transfer_t* trsf, size_t value, size_t max_value)
{
    if (trsf != NULL)
        gmio_task_iface_handle_progress(&trsf->task_iface, value, max_value);
}

#endif /* GMIO_INTERNAL_HELPER_TRANSFER_H */
