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

#pragma once

#include "../global.h"
#include <string>

namespace gmio {

//! Returns whether 'c' is a space or not
constexpr bool ascii_isSpace(char c);

//! Returns whether 'c' is a digit or not
constexpr bool ascii_isDigit(char c);

//! Returns whether 'c' is an uppercase letter or not
constexpr bool ascii_isUpper(char c);

//! Returns whether 'c' is an lowercase letter or not
constexpr bool ascii_isLower(char c);

//! Returns the lowercase letter converted to uppercase
constexpr char ascii_toUpper(char c);

//! Returns the uppercase letter converted to lowercase
constexpr char ascii_toLower(char c);

//! Returns whether 'c1' compare equals to 'c2'.
//! \note Comparison is case-insensitive
constexpr bool ascii_iequals(char c1, char c2);

//! Returns 0 if 'str1' and 'str2' compare equal, non-zero otherwise.
//! \note Comparison is case-insensitive
inline int ascii_stricmp(const char* str1, const char* str2);

//! Returns 0 if the first 'n' characters of 'str1' and 'str2' compare equal,
//! non-zero otherwise.
//! \note Comparison is case-insensitive
inline int ascii_strincmp(const char* str1, const char* str2, size_t n);

//! Returns true if 'str' starts with string 'begin'.
//! \note Comparison is case-insensitive
inline bool ascii_istartsWith(const char* str, const char* begin);

//! Locate substring (insensitive case string matching).
//! Behaves the same as strstr()
const char* ascii_istrstr(const char *str1, const char *str2);

//! Trim whitespaces in string 'str' from end
inline void ascii_trimEnd(char* str, size_t len);

inline void ascii_trimEnd(std::string* str);

//
// -- Implementation
//

constexpr bool ascii_isSpace(char c)
{
    // 0x20 : space (SPC)
    // 0x09 : horizontal tab (TAB)
    // 0x0a : newline (LF)
    // 0x0b : vertical tab (VT)
    // 0x0c : feed (FF)
    // 0x0d : carriage return (CR)
    //
    // TODO: use table of property bits
    return c == 0x20 || ((uint8_t)(c - 0x09) < 5);
}

constexpr bool ascii_isDigit(char c)
{
    // 48 <= c <= 57
    return (uint8_t)(c - 48) < 10;
}

constexpr bool ascii_isUpper(char c)
{
    // 65 <= c <= 90;
    return (uint8_t)(c - 65) < 26;
}

constexpr bool ascii_isLower(char c)
{
    // 97 <= c <= 122;
    return (uint8_t)(c - 97) < 26;
}

static const char ascii_tableUpper[128] = {
    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,0x09,0x0A,  0 ,  0 ,0x0D,  0 ,  0 ,
    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    ' ', '!', '"', '#', '$', '%', '&','\'', '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[','\\', ']', '^', '_',
    '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~', 0,
};

constexpr char ascii_toUpper(char c)
{
    return ascii_tableUpper[(unsigned char)c];
}

static const char ascii_tableLower[128] = {
    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,0x09,0x0A,  0 ,  0 ,0x0D,  0 ,  0 ,
    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    ' ', '!', '"', '#', '$', '%', '&','\'', '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
    '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[','\\', ']', '^', '_',
    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', 0,
};

constexpr char ascii_toLower(char c)
{
    return ascii_tableLower[(unsigned char)c];
}

constexpr bool ascii_iequals(char c1, char c2)
{
    // TODO: eliminate branch
    return c1 == c2 || (ascii_toUpper(c1) == ascii_toUpper(c2));
}

int ascii_stricmp(const char* str1, const char* str2)
{
    while (*str1 != '\0' && ascii_iequals(*str1, *str2)) {
        ++str1;
        ++str2;
    }
    return *str1 - *str2;
}

int ascii_strincmp(const char* str1, const char* str2, size_t n)
{
    while (n > 0 && *str1 != 0 && ascii_iequals(*str1, *str2)) {
        ++str1;
        ++str2;
        --n;
    }
    return ascii_toLower(*str1) - ascii_toLower(*str2);
}

bool ascii_istartsWith(const char* str, const char* begin)
{
    while (*begin != 0) {
        if (*str == 0 || !ascii_iequals(*str, *begin))
            return false;
        ++str;
        ++begin;
    }
    return true;
}

void ascii_trimEnd(char* str, size_t len)
{
    if (len > 0) {
        do {
            --len;
            if (str[len] == '\0' || ascii_isSpace(str[len]))
                str[len] = '\0';
            else
                return;
        } while (len != 0);
    }
}

void ascii_trimEnd(std::string* str)
{
    if (!str->empty()) {
        auto it_begin = str->begin();
        auto it_erase_end = str->end();
        auto it = it_erase_end;
        while (it != it_begin && ascii_isSpace(*(--it)));
        const auto it_erase_begin = !ascii_isSpace(*it) ? it + 1 : it;
        str->erase(it_erase_begin, it_erase_end);
    }
}

} // namespace gmio
