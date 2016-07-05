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

#ifndef GMIO_INTERNAL_STRING_H
#define GMIO_INTERNAL_STRING_H

#include "../global.h"

#include <stddef.h>

/*! Stores a read-only string of 8-bit chars
 *
 *  For faster lookups, it knowns the length of its contents.
 */
struct gmio_const_string
{
    const char* ptr; /*!< Contents */
    size_t len;      /*!< Size(length) of current contents */
};

/*! Stores a mutable string of 8-bit chars
 *
 *  For faster lookups, it knowns the length of its contents. Length must not
 *  exceeds the maximum size(capacity).
 */
struct gmio_string
{
    char*  ptr; /*!< Contents */
    size_t len; /*!< Size(length) of current contents */
    size_t capacity; /*!< Maximum contents size */
};

/*! Expands to bracket initialization of a gmio_const_string from const char[]
 *
 *  Example:
 *  \code{.c}
 *      static const char token[] = "woops";
 *      struct gmio_const_string token_s = GMIO_CONST_STRING_FROM_ARRAY(token);
 *  \endcode
 */
#define GMIO_CONST_STRING_FROM_ARRAY(array) { &(array)[0], sizeof(array) - 1 }

/*! Returns an initialized struct gmio_const_string object */
GMIO_INLINE struct gmio_const_string gmio_const_string(const char* ptr, size_t len);

/*! Returns an initialized struct gmio_string object
 *
 *  gmio_string::capacity is set to <tt>max(len,capacity)</tt>
 */
GMIO_INLINE struct gmio_string gmio_string(char* ptr, size_t len, size_t capacity);

/*! Clears the contents of the string \p str and makes it null
 *
 *  \warning Memory pointed to by gmio_string::ptr is not freed in any way
 */
GMIO_INLINE void gmio_string_clear(struct gmio_string* str);

/*! Returns a pointer after the last character of \p str */
GMIO_INLINE const char* gmio_string_end(const struct gmio_string* str);

/*! Copies contents of \p src into \p dst */
GMIO_INLINE void gmio_string_copy(
        struct gmio_string* dst, const struct gmio_string* src);

/*! Copies contents of C-string \p src into \p dst */
GMIO_INLINE char* gmio_cstr_copy(
        char* dst, size_t dst_capacity, const char* src, size_t src_len);

/*
 * -- Implementation
 */

#include <string.h>
#include "min_max.h"

struct gmio_const_string gmio_const_string(const char* ptr, size_t len)
{
    struct gmio_const_string cstr;
    cstr.ptr = ptr;
    cstr.len = len;
    return cstr;
}

struct gmio_string gmio_string(char* ptr, size_t len, size_t capacity)
{
    struct gmio_string str;
    str.ptr = ptr;
    str.len = len;
    str.capacity = GMIO_MAX(len, capacity);
    return str;
}

void gmio_string_clear(struct gmio_string* str)
{
    str->ptr[0] = 0;
    str->len = 0;
}

const char* gmio_string_end(const struct gmio_string* str)
{
    return &str->ptr[str->len];
}

void gmio_string_copy(
        struct gmio_string* dst, const struct gmio_string* src)
{
    const size_t dst_new_len = GMIO_MIN(dst->capacity, src->len);
    strncpy(dst->ptr, src->ptr, dst_new_len);
    dst->len = dst_new_len;
}

char* gmio_cstr_copy(
        char* dst, size_t dst_capacity, const char* src, size_t src_len)
{
    const size_t copy_len =
            dst_capacity > 0 ?
                GMIO_MIN(dst_capacity - 1, src_len) :
                0;
    if (copy_len > 0) {
        strncpy(dst, src, copy_len);
        dst[copy_len] = '\0';
    }
    return dst + copy_len;
}

#endif /* GMIO_INTERNAL_STRING_H */
