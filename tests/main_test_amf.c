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

#include "utest_lib.h"

#include "../src/gmio_core/global.h"
#include "../src/gmio_core/memblock.h"

#include "test_amf_io.c"

#include <stddef.h>

/* Static memblock */
struct gmio_memblock gmio_memblock_for_tests()
{
    return gmio_memblock_malloc(512 * 1024); /* 512KB */
}

const char* all_tests()
{
    UTEST_SUITE_START();

    gmio_memblock_set_default_constructor(gmio_memblock_for_tests);

    UTEST_RUN(test_amf_write_doc_null);
    UTEST_RUN(test_amf_write_doc_1_plaintext);
    UTEST_RUN(test_amf_write_doc_1_zip);
    UTEST_RUN(test_amf_write_doc_1_zip64);
    UTEST_RUN(test_amf_write_doc_1_zip64_file);
    UTEST_RUN(test_amf_write_doc_1_task_iface);

    return NULL;
}
UTEST_MAIN(all_tests)
