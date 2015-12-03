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

#ifndef GMIO_INTERNAL_STL_FUNPTR_TYPEDEFS_H
#define GMIO_INTERNAL_STL_FUNPTR_TYPEDEFS_H

#include <stddef.h>

#include "../stl_mesh.h"
#include "../stl_mesh_creator.h"

/* gmio_stl_triangle */
typedef void (*gmio_stl_triangle_func_fix_endian_t)(struct gmio_stl_triangle*);

/* gmio_stl_mesh */
typedef void (*gmio_stl_mesh_func_get_triangle_t)(
        const void*, uint32_t, struct gmio_stl_triangle*);

/* gmio_stl_mesh_creator */
typedef void (*gmio_stl_mesh_creator_func_ascii_begin_solid_t)(
        void*, gmio_streamsize_t, const char*);
typedef void (*gmio_stl_mesh_creator_func_binary_begin_solid_t)(
        void*, uint32_t, const struct gmio_stlb_header*);
typedef void (*gmio_stl_mesh_creator_func_add_triangle_t)(
        void*, uint32_t, const struct gmio_stl_triangle*);
typedef void (*gmio_stl_mesh_creator_func_end_solid_t)(void*);

#endif /* GMIO_INTERNAL_STL_FUNPTR_TYPEDEFS_H */
