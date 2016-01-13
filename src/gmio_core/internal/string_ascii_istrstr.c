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

#include "string_ascii_utils.h"

static int gmio_ascii_ustrincmp(
        const unsigned char* str1, const unsigned char* str2, size_t n)
{
    return gmio_ascii_strincmp((const char*)str1, (const char*)str2, n);
}

#define RETURN_TYPE const char*
#define AVAILABLE(h, h_l, j, n_l) \
            (!memchr ((h) + (h_l), '\0', (j) + (n_l) - (h_l)) \
             && ((h_l) = (j) + (n_l)))
#define CANON_ELEMENT(c) ((int)gmio_ascii_tolower(c))
#define CMP_FUNC gmio_ascii_ustrincmp
#include "eblake_str_two_way.h"

const char *gmio_ascii_istrstr(const char *str1, const char *str2)
{
    /* Guaranteed linear performance */
    const char* haystack = str1;
    const char* needle = str2;
    size_t needle_len; /* Length of NEEDLE */
    size_t haystack_len; /* Known minimum length of HAYSTACK */
    int ok = 1; /* True if NEEDLE is prefix of HAYSTACK */

    /* Determine length of NEEDLE, and in the process, make sure
       HAYSTACK is at least as long (no point processing all of a long
       NEEDLE if HAYSTACK is too short) */
    while (*haystack && *needle) {
        ok &= (gmio_ascii_tolower((unsigned char) *haystack++)
               == gmio_ascii_tolower((unsigned char) *needle++));
    }

    if (*needle)
        return NULL;
    if (ok)
        return str1;
    needle_len = needle - str2;
    haystack = str1 + 1;
    haystack_len = needle_len - 1;

    /* Perform the search */
    if (needle_len < LONG_NEEDLE_THRESHOLD) {
        return two_way_short_needle(
                    (const unsigned char*) haystack,
                    haystack_len,
                    (const unsigned char*) str2, needle_len);
    }
    return two_way_long_needle(
                (const unsigned char*) haystack,
                haystack_len,
                (const unsigned char*) str2,
                needle_len);
}
