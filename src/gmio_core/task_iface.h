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

/*! \file task_iface.h
 *  Declaration of gmio_task_iface
 *
 *  \addtogroup gmio_core
 *  @{
 */

#pragma once

#include "global.h"

/*! Defines an interface through which a task can be controlled */
struct gmio_task_iface
{
    /*! Optional opaque pointer on a user task object, passed as first
     *  argument to hook functions */
    void* cookie;

    /*! Optional function that says if the currently running task must stop
     *
     *  If \c true is returned then the current task should abort as
     *  soon as possible, otherwise it can continue execution */
    bool (*func_is_stop_requested)(void* cookie);

    /*! Optional function that is called anytime some new progress was done
     *
     *  \param cookie The cookie set inside the gmio_task_iface object
     *  \param value Current value of the task progress (<= \p max_value )
     *  \param max_value Maximum value of the task progress */
    void (*func_handle_progress)(
            void* cookie, intmax_t value, intmax_t max_value);
};

/*! @} */
