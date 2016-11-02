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

#include "locale_utils.h"

#include "../error.h"
#include "string_ascii_utils.h"

#include <locale.h>
#include <stddef.h>
#include <string.h>

const char *gmio_lc_numeric()
{
    const char* lcnum = setlocale(LC_NUMERIC, NULL);
    return lcnum != NULL ? lcnum : "";
}

bool gmio_lc_numeric_is_C()
{
    const char* lc = gmio_lc_numeric();
    return (gmio_ascii_stricmp(lc, "C") == 0
            || gmio_ascii_stricmp(lc, "POSIX") == 0);
}

static char global_lc_numeric[64] = {0};

void gmio_lc_numeric_save()
{
    /* Save LC_NUMERIC
     * POSIX specifies that the pointer returned by setlocale(), not just the
     * contents of the pointed-to string, may be invalidated by subsequent calls
     * to setlocale */
    strncpy(global_lc_numeric,
            setlocale(LC_NUMERIC, NULL),
            GMIO_ARRAY_SIZE(global_lc_numeric));
}

void gmio_lc_numeric_restore()
{
    if (global_lc_numeric[0] != '\0')
        setlocale(LC_NUMERIC, global_lc_numeric);
}
