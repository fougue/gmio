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

#ifndef GMIO_INTERNAL_STRING_ASCII_UTILS_H
#define GMIO_INTERNAL_STRING_ASCII_UTILS_H

#include "../global.h"

#include <stddef.h>

/*! Returns non-zero if \p c is a space, zero otherwise */
GMIO_INLINE int gmio_ascii_isspace(char c);

/*! Returns non-zero if \p c is a digit, zero otherwise */
GMIO_INLINE int gmio_ascii_isdigit(char c);

/*! Returns non-zero if \p c is an uppercase letter, zero otherwise */
GMIO_INLINE int gmio_ascii_isupper(char c);

/*! Returns non-zero if \p c is a lowercase letter, zero otherwise */
GMIO_INLINE int gmio_ascii_islower(char c);

/*! Returns the lowercase letter converted to uppercase */
GMIO_INLINE char gmio_ascii_toupper(char c);

/*! Returns the uppercase letter converted to lowercase */
GMIO_INLINE char gmio_ascii_tolower(char c);

/*! Returns 0 if \p c1 compare equals to \p c2, non-zero otherwise.
 *  \note Comparison is case-insensitive */
GMIO_INLINE bool gmio_ascii_char_iequals(char c1, char c2);

/*! Returns 0 if \p str1 and \p str2 compare equal, non-zero otherwise.
 *  \note Comparison is case-insensitive */
GMIO_INLINE int gmio_ascii_stricmp(const char* str1, const char* str2);

/*! Returns 0 if the first \p n characters of \p str1 and \p str2 compare equal,
 *  non-zero otherwise.
 *  \note Comparison is case-insensitive */
GMIO_INLINE int gmio_ascii_strincmp(
        const char* str1, const char* str2, size_t n);

/*! Returns true if \p str starts with string \p begin.
 *  \note Comparison is case-insensitive */
GMIO_INLINE bool gmio_ascii_istarts_with(
        const char* str, const char* begin);

/*! Locate substring (insensitive case string matching).
 *  Behaves the same as strstr() */
const char* gmio_ascii_istrstr(const char *str1, const char *str2);


/*
 * -- Implementation
 */

#include <string.h>

#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
#  include <ctype.h>
#endif

int gmio_ascii_isspace(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return isspace(c);
#else
    /* 0x20 : space (SPC)
     * 0x09 : horizontal tab (TAB)
     * 0x0a : newline (LF)
     * 0x0b : vertical tab (VT)
     * 0x0c : feed (FF)
     * 0x0d : carriage return (CR)
     *
     * TODO: eliminate branch
     */
    return c == 0x20 || ((uint8_t)(c - 0x09) < 5);
#endif
}

int gmio_ascii_isdigit(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return isdigit(c);
#else
    /* 48 <= c <= 57 */
    return (uint8_t)(c - 48) < 10;
#endif
}

int gmio_ascii_isupper(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return isupper(c);
#else
    /* 65 <= c <= 90; */
    return (uint8_t)(c - 65) < 26;
#endif
}

int gmio_ascii_islower(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return islower(c);
#else
    /* 97 <= c <= 122; */
    return (uint8_t)(c - 97) < 26;
#endif
}

char gmio_ascii_toupper(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return (char)toupper(c);
#else
    static const char table_toupper[128] = {
        0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,0x09,0x0A,  0 ,  0 ,0x0D,  0 ,  0 ,
        0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
        ' ', '!', '"', '#', '$', '%', '&','\'', '(', ')', '*', '+', ',', '-', '.', '/',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
        '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
        'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[','\\', ']', '^', '_',
        '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
        'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~', 0,
    };
    return table_toupper[(unsigned char)c];
#endif
}

char gmio_ascii_tolower(char c)
{
#ifdef GMIO_STRING_ASCII_UTILS_CTYPE_H
    return (char)tolower(c);
#else
    static const char table_tolower[128] = {
        0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,0x09,0x0A,  0 ,  0 ,0x0D,  0 ,  0 ,
        0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
        ' ', '!', '"', '#', '$', '%', '&','\'', '(', ')', '*', '+', ',', '-', '.', '/',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
        '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
        'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[','\\', ']', '^', '_',
        '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
        'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', 0,
    };
    return table_tolower[(unsigned char)c];
#endif
}

bool gmio_ascii_char_iequals(char c1, char c2)
{
    /* TODO: eliminate branch */
    return c1 == c2 || (gmio_ascii_toupper(c1) == gmio_ascii_toupper(c2));
}

int gmio_ascii_stricmp(const char* str1, const char* str2)
{
    while (*str1 != 0 && gmio_ascii_char_iequals(*str1, *str2)) {
        ++str1;
        ++str2;
    }
    return *str1 - *str2;
}

int gmio_ascii_strincmp(const char* str1, const char* str2, size_t n)
{
    while (n > 0 && *str1 != 0 && gmio_ascii_char_iequals(*str1, *str2)) {
        ++str1;
        ++str2;
        --n;
    }
    return gmio_ascii_tolower(*str1) - gmio_ascii_tolower(*str2);
}

bool gmio_ascii_istarts_with(const char* str, const char* begin)
{
    while (*begin != 0) {
        if (*str == 0 || !gmio_ascii_char_iequals(*str, *begin))
            return false;
        ++str;
        ++begin;
    }
    return true;
}

#endif /* GMIO_INTERNAL_STRING_ASCII_UTILS_H */
