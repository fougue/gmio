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
