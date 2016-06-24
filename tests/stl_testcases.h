/****************************************************************************
** gmio
** Copyright Fougue (24 Jun. 2016)
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

#ifndef GMIO_TESTS_STL_TESTCASES_H
#define GMIO_TESTS_STL_TESTCASES_H

#include "../src/gmio_core/error.h"
#include "../src/gmio_core/stream.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_format.h"

struct stl_read_testcase
{
    const char* filepath;
    int errorcode; /* Expected read error code */
    enum gmio_stl_format format;
    const char* solid_name;
    uint32_t expected_facet_count;
    gmio_streamsize_t expected_size; /* -1: check against actual file size */
};

const struct stl_read_testcase* stl_read_testcases_ptr();
const struct stl_read_testcase* stl_read_testcases_ptr_end();
size_t stl_read_testcases_size();

extern const char filepath_stlb_grabcad_arm11[];
extern const char filepath_stlb_header_nofacets[];
extern const char filepath_stlb_empty[];
extern const char filepath_stla_4meshs[];
extern const char filepath_stlb_4meshs[];

#endif /* GMIO_TESTS_STL_TESTCASES_H */
