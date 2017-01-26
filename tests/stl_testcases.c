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

#include "stl_testcases.h"

const char filepath_stlb_grabcad_arm11[] =
        "models/solid_grabcad_arm11_link0_hb.le_stlb";
const char filepath_stlb_header_nofacets[] =
        "models/solid_header_no_facets.le_stlb";
const char filepath_stlb_empty[] = "models/solid_empty.stlb";
const char filepath_stla_4meshs[] = "models/solid_4meshs.stla";
const char filepath_stlb_4meshs[] = "models/solid_4meshs.le_stlb";

const struct stl_read_testcase g_stl_read_testcases[] = {
    { "models/file_empty", GMIO_STL_ERROR_UNKNOWN_FORMAT,
      GMIO_STL_FORMAT_UNKNOWN, NULL, 0, 0
    },
    { "models/solid_4vertex.stla", GMIO_STL_ERROR_PARSING,
      GMIO_STL_FORMAT_ASCII, NULL, 1, -1
    },
    { "models/solid_anonymous_empty.stla", GMIO_ERROR_OK,
      GMIO_STL_FORMAT_ASCII, NULL, 0, -1
    },
    { "models/solid_empty.stla", GMIO_ERROR_OK,
      GMIO_STL_FORMAT_ASCII, "emptysolid", 0, -1
    },
    { filepath_stlb_empty, GMIO_ERROR_OK,
      GMIO_STL_FORMAT_BINARY_LE, NULL, 0, -1
    },
    { "models/solid_empty_name_many_words.stla", GMIO_ERROR_OK,
      GMIO_STL_FORMAT_ASCII, "name with multiple words", 0, -1
    },
    { "models/solid_empty_name_many_words_single_letters.stla", GMIO_ERROR_OK,
      GMIO_STL_FORMAT_ASCII, "a b c d e f\t\tg h", 0, -1
    },
    { filepath_stlb_grabcad_arm11, GMIO_ERROR_OK,
      GMIO_STL_FORMAT_BINARY_LE, NULL, 1436, -1
    },
    { filepath_stlb_header_nofacets, GMIO_STL_ERROR_UNKNOWN_FORMAT,
      GMIO_STL_FORMAT_UNKNOWN, NULL, 0, 0
    },
    { "models/solid_jburkardt_sphere.stla", GMIO_ERROR_OK,
      GMIO_STL_FORMAT_ASCII, "sphere", 228, -1
    },
    { "models/solid_lack_z.stla", GMIO_STL_ERROR_PARSING,
      GMIO_STL_FORMAT_ASCII, NULL, 1, -1
    },
    { "models/solid_one_facet.stla", GMIO_ERROR_OK,
      GMIO_STL_FORMAT_ASCII, NULL, 1, -1
    },
    { "models/solid_one_facet.le_stlb", GMIO_ERROR_OK,
      GMIO_STL_FORMAT_BINARY_LE, NULL, 1, -1
    },
    { "models/solid_one_facet.be_stlb", GMIO_ERROR_OK,
      GMIO_STL_FORMAT_BINARY_BE, NULL, 1, -1
    },
    { "models/solid_one_facet_uppercase.stla", GMIO_ERROR_OK,
      GMIO_STL_FORMAT_ASCII, NULL, 1, -1
    }
};

const struct stl_read_testcase *stl_read_testcases_ptr()
{
    return g_stl_read_testcases;
}

const struct stl_read_testcase *stl_read_testcases_ptr_end()
{
    return stl_read_testcases_ptr() + stl_read_testcases_size();
}

size_t stl_read_testcases_size()
{
    return GMIO_ARRAY_SIZE(g_stl_read_testcases);
}
