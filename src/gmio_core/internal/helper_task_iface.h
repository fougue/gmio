/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#ifndef GMIO_INTERNAL_HELPER_TASK_IFACE_H
#define GMIO_INTERNAL_HELPER_TASK_IFACE_H

#include "../task_iface.h"

#include <stddef.h>

/*! Safe and convenient function for gmio_task_iface::func_is_stop_requested() */
GMIO_INLINE bool gmio_task_iface_is_stop_requested(
        const struct gmio_task_iface* itask)
{
    if (itask != NULL && itask->func_is_stop_requested != NULL)
        return itask->func_is_stop_requested(itask->cookie);
    return false;
}

/*! Safe and convenient function for gmio_task_iface::func_handle_progress() */
GMIO_INLINE void gmio_task_iface_handle_progress(
        const struct gmio_task_iface* itask, intmax_t value, intmax_t max_value)
{
    if (itask != NULL && itask->func_handle_progress != NULL)
        itask->func_handle_progress(itask->cookie, value, max_value);
}

#endif /* GMIO_INTERNAL_HELPER_TASK_IFACE_H */
