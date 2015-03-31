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

#ifndef GMIO_INTERNAL_HELPER_TRANSFER_H
#define GMIO_INTERNAL_HELPER_TRANSFER_H

#include "../transfer.h"

#include <stddef.h>

/*! Safe and convenient function for gmio_transfer::is_stop_requested_func() */
GMIO_INLINE gmio_bool_t gmio_transfer_is_stop_requested(
        const gmio_transfer_t* trsf)
{
    if (trsf != NULL && trsf->is_stop_requested_func != NULL)
        return trsf->is_stop_requested_func(trsf->cookie);
    return GMIO_FALSE;
}

/*! Safe and convenient function for gmio_transfer::handle_progress_func() */
GMIO_INLINE void gmio_transfer_handle_progress(
        const gmio_transfer_t* trsf, size_t value, size_t max_value)
{
    if (trsf != NULL && trsf->handle_progress_func != NULL)
        trsf->handle_progress_func(trsf->cookie, value, max_value);
}

#endif /* GMIO_INTERNAL_HELPER_TRANSFER_H */
