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

#include "utest_assert.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_stl/stl_infos.h"

#include <stdio.h>

static const char stl_jburkardt_sphere_filepath[] =
        "models/solid_jburkardt_sphere.stla";

const char* test_stl_infos()
{
    FILE* stla_file = fopen(stl_jburkardt_sphere_filepath, "rb");
    struct gmio_stl_infos_get_args args = {0};
    struct gmio_stl_infos infos = {0};
    int error = GMIO_ERROR_OK;

    args.format = GMIO_STL_FORMAT_ASCII;
    args.memblock = gmio_memblock_malloc(8 * 1024); /* 8Ko */
    args.stream = gmio_stream_stdio(stla_file);

    error = gmio_stl_infos_get(&args, &infos, GMIO_STL_INFO_FLAG_ALL);

    gmio_memblock_deallocate(&args.memblock);
    fclose(stla_file);

    UTEST_ASSERT(error == GMIO_ERROR_OK);
    /*UTEST_ASSERT(infos.facet_count == 228);*/
    /*UTEST_ASSERT(stats.size == 54297);*/

    return NULL;
}
