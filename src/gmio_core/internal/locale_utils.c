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

bool gmio_check_lc_numeric(int *error)
{
    if (!gmio_lc_numeric_is_C())
        *error = GMIO_ERROR_BAD_LC_NUMERIC;
    return gmio_no_error(*error);
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
