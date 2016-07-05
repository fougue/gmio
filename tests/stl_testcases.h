/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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
