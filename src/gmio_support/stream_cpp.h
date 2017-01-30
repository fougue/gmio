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

/*! \file stream_cpp.h
 *  Support of standard C++ streams
 *
*  \addtogroup gmio_support
 *  @{
 */

#ifndef __cplusplus
#  error C++ compiler required
#endif

#pragma once

#include "support_global.h"
#include "../gmio_core/stream.h"
#include <cstring>
#include <iostream>

/*! Returns a gmio_stream for C++ input stream (cookie will hold \p s ) */
template<typename CHAR, typename TRAITS>
gmio_stream gmio_istream_cpp(std::basic_istream<CHAR, TRAITS>* s);

/*! Returns a gmio_stream for C++ output stream (cookie will hold \p s ) */
template<typename CHAR, typename TRAITS>
gmio_stream gmio_ostream_cpp(std::basic_ostream<CHAR, TRAITS>* s);




//
// Implementation
//
#ifndef DOXYGEN

namespace gmio {
namespace internal {

template<typename STREAM>
bool stream_cpp_at_end(void* cookie)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    return s->eof();
}

template<typename STREAM>
int stream_cpp_error(void* cookie)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    return s->rdstate();
}

template<typename STREAM>
size_t istream_cpp_read(
        void* cookie, void* ptr, size_t item_size, size_t item_count)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    s->read(static_cast<char*>(ptr), item_size * item_count);
    return static_cast<size_t>(s->gcount() / item_size);
}

template<typename STREAM>
size_t ostream_cpp_write(
        void* cookie, const void* ptr, size_t item_size, size_t item_count)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    s->write(static_cast<const char*>(ptr), item_size * item_count);
    // TODO: return the number of bytes actually written
    return item_size * item_count;
}

template<typename STREAM>
gmio_streamsize_t istream_cpp_size(void* cookie)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    std::streampos pos = s->tellg();
    s->seekg(0, std::ios_base::beg);
    std::streampos begin_pos = s->tellg();
    s->seekg(0, std::ios_base::end);
    std::streampos end_pos = s->tellg();
    s->seekg(pos, std::ios_base::beg); // Restore pos
    return end_pos - begin_pos;
}

template<typename STREAM>
gmio_streamsize_t ostream_cpp_size(void* cookie)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    std::streampos pos = s->tellp();
    s->seekp(0, std::ios_base::beg);
    std::streampos begin_pos = s->tellp();
    s->seekp(0, std::ios_base::end);
    std::streampos end_pos = s->tellp();
    s->seekp(pos, std::ios_base::beg); // Restore pos
    return end_pos - begin_pos;
}

GMIO_INLINE void copy_cpp_streampos(gmio_streampos* pos, std::streampos spos)
{
    std::memcpy(&pos->cookie[0], &spos, sizeof(std::streampos));
}

GMIO_INLINE std::streampos to_cpp_streampos(const gmio_streampos* pos)
{
    std::streampos spos;
    std::memcpy(&spos, &pos->cookie[0], sizeof(std::streampos));
    return spos;
}

template<typename STREAM>
int istream_cpp_get_pos(void* cookie, gmio_streampos* pos)
{
    copy_cpp_streampos(pos, static_cast<STREAM*>(cookie)->tellg());
    return 0;
}

template<typename STREAM>
int istream_cpp_set_pos(void* cookie, const gmio_streampos* pos)
{
    static_cast<STREAM*>(cookie)->seekg(to_cpp_streampos(pos));
    return 0; // TODO: return error code
}

template<typename STREAM>
int ostream_cpp_get_pos(void* cookie, gmio_streampos* pos)
{
    copy_cpp_streampos(pos, static_cast<STREAM*>(cookie)->tellp());
    return 0;
}

template<typename STREAM>
static int ostream_cpp_set_pos(void* cookie, const gmio_streampos* pos)
{
    static_cast<STREAM*>(cookie)->seekp(to_cpp_streampos(pos));
    return 0; // TODO: return error code
}

template<typename STREAM>
void stream_cpp_init_common(STREAM* s, gmio_stream* stream)
{
    *stream = gmio_stream_null();
    stream->cookie = s;
    stream->func_at_end = gmio::internal::stream_cpp_at_end<STREAM>;
    stream->func_error = gmio::internal::stream_cpp_error<STREAM>;
}

} // namespace internal
} // namespace gmio

template<typename CHAR, typename TRAITS>
gmio_stream gmio_istream_cpp(std::basic_istream<CHAR, TRAITS>* s)
{
    typedef std::basic_istream<CHAR, TRAITS> CppStream;
    gmio_stream stream;
    gmio::internal::stream_cpp_init_common(s, &stream);
    stream.func_size = gmio::internal::istream_cpp_size<CppStream>;
    stream.func_read = gmio::internal::istream_cpp_read<CppStream>;
    stream.func_get_pos = gmio::internal::istream_cpp_get_pos<CppStream>;
    stream.func_set_pos = gmio::internal::istream_cpp_set_pos<CppStream>;
    return stream;
}

template<typename CHAR, typename TRAITS>
gmio_stream gmio_ostream_cpp(std::basic_ostream<CHAR, TRAITS>* s)
{
    typedef std::basic_ostream<CHAR, TRAITS> CppStream;
    gmio_stream stream;
    gmio::internal::stream_cpp_init_common(s, &stream);
    stream.func_size = gmio::internal::ostream_cpp_size<CppStream>;
    stream.func_write = gmio::internal::ostream_cpp_write<CppStream>;
    stream.func_get_pos = gmio::internal::ostream_cpp_get_pos<CppStream>;
    stream.func_set_pos = gmio::internal::ostream_cpp_set_pos<CppStream>;
    return stream;
}

#endif // !DOXYGEN

/*! @} */
