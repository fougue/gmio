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
