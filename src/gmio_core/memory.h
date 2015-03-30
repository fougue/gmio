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

#ifndef GMIO_MEMORY_H
#define GMIO_MEMORY_H

#include <string.h>

/*! Type alias for "pointer on function that allocates memory" (like standard malloc()) */
typedef void* (*gmio_malloc_func_t)(size_t);

/*! Type alias for "pointer on function that frees memory" (like standard free()) */
typedef void  (*gmio_free_func_t)(void*);

#endif /* GMIO_MEMORY_H */
