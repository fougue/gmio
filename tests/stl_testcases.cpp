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

namespace gmio {

const char filepath_stlb_grabcad_arm11[] =
        "models/solid_grabcad_arm11_link0_hb.le_stlb";
const char filepath_stlb_header_nofacets[] =
        "models/solid_header_no_facets.le_stlb";
const char filepath_stlb_empty[] = "models/solid_empty.stlb";
const char filepath_stla_4meshs[] = "models/solid_4meshs.stla";
const char filepath_stlb_4meshs[] = "models/solid_4meshs.le_stlb";

const STL_ReadTestCase g_stl_read_testcases[] = {
    { "models/file_empty", Error_OK,
      STL_Format_Unknown, nullptr, 0, 0
    },
    { "models/solid_4vertex.stla", STL_Error_Parsing,
      STL_Format_Ascii, nullptr, 1, -1
    },
    { "models/solid_anonymous_empty.stla", Error_OK,
      STL_Format_Ascii, nullptr, 0, -1
    },
    { "models/solid_empty.stla", Error_OK,
      STL_Format_Ascii, "emptysolid", 0, -1
    },
    { filepath_stlb_empty, Error_OK,
      STL_Format_BinaryLittleEndian, nullptr, 0, -1
    },
    { "models/solid_empty_name_many_words.stla", Error_OK,
      STL_Format_Ascii, "name with multiple words", 0, -1
    },
    { "models/solid_empty_name_many_words_single_letters.stla", Error_OK,
      STL_Format_Ascii, "a b c d e f\t\tg h", 0, -1
    },
    { filepath_stlb_grabcad_arm11, Error_OK,
      STL_Format_BinaryLittleEndian, nullptr, 1436, -1
    },
    { filepath_stlb_header_nofacets, STL_Error_UnknownFormat,
      STL_Format_Unknown, nullptr, 0, 0
    },
    { "models/solid_jburkardt_sphere.stla", Error_OK,
      STL_Format_Ascii, "sphere", 228, -1
    },
    { "models/solid_lack_z.stla", STL_Error_Parsing,
      STL_Format_Ascii, nullptr, 1, -1
    },
    { "models/solid_one_facet.stla", Error_OK,
      STL_Format_Ascii, nullptr, 1, -1
    },
    { "models/solid_one_facet.le_stlb", Error_OK,
      STL_Format_BinaryLittleEndian, nullptr, 1, -1
    },
    { "models/solid_one_facet.be_stlb", Error_OK,
      STL_Format_BinaryBigEndian, nullptr, 1, -1
    },
    { "models/solid_one_facet_uppercase.stla", Error_OK,
      STL_Format_Ascii, nullptr, 1, -1
    }
};

Span<const STL_ReadTestCase> STL_spanReadTestCase()
{
    return makeSpan(g_stl_read_testcases);
}

} // namespace gmio
