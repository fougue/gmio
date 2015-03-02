/****************************************************************************
**
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
**
****************************************************************************/

#include "task_control.h"

#include <stddef.h>

gmio_bool_t gmio_task_control_is_stop_requested(const gmio_task_control_t* task_ctrl)
{
  if (task_ctrl != NULL && task_ctrl->is_stop_requested_func != NULL)
    return task_ctrl->is_stop_requested_func(task_ctrl->cookie);
  return GMIO_FALSE;
}
