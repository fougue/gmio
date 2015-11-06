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

/*! \file stream_cpp.h
 *  Support of standard C++ streams
 *
*  \addtogroup gmio_support
 *  @{
 */

#ifndef __cplusplus
#  error C++ compiler required
#endif

#ifndef GMIO_SUPPORT_STREAM_CPP_H
#define GMIO_SUPPORT_STREAM_CPP_H

#include "support_global.h"
#include "../gmio_core/stream.h"

#include <cstring>
#include <iostream>

/*! Returns a gmio_stream for C++ input stream (cookie will hold \p s ) */
template<typename CHAR, typename TRAITS>
gmio_stream_t gmio_istream_cpp(std::basic_istream<CHAR, TRAITS>* s);

/*! Returns a gmio_stream for C++ output stream (cookie will hold \p s ) */
template<typename CHAR, typename TRAITS>
gmio_stream_t gmio_ostream_cpp(std::basic_ostream<CHAR, TRAITS>* s);




//
// Implementation
//
#ifndef DOXYGEN

namespace gmio {
namespace internal {

template<typename STREAM>
gmio_bool_t stream_cpp_at_end(void* cookie)
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

GMIO_INLINE void copy_cpp_streampos(gmio_stream_pos_t* pos, std::streampos spos)
{
    std::memcpy(&pos->cookie[0], &spos, sizeof(std::streampos));
}

GMIO_INLINE std::streampos to_cpp_streampos(const gmio_stream_pos_t* pos)
{
    std::streampos spos;
    std::memcpy(&spos, &pos->cookie[0], sizeof(std::streampos));
    return spos;
}

template<typename STREAM>
int istream_cpp_get_pos(void* cookie, gmio_stream_pos_t* pos)
{
    copy_cpp_streampos(pos, static_cast<STREAM*>(cookie)->tellg());
    return 0;
}

template<typename STREAM>
int istream_cpp_set_pos(void* cookie, const gmio_stream_pos_t* pos)
{
    static_cast<STREAM*>(cookie)->seekg(to_cpp_streampos(pos));
    return 0; // TODO: return error code
}

template<typename STREAM>
int ostream_cpp_get_pos(void* cookie, gmio_stream_pos_t* pos)
{
    copy_cpp_streampos(pos, static_cast<STREAM*>(cookie)->tellp());
    return 0;
}

template<typename STREAM>
static int ostream_cpp_set_pos(void* cookie, const gmio_stream_pos_t* pos)
{
    static_cast<STREAM*>(cookie)->seekp(to_cpp_streampos(pos));
    return 0; // TODO: return error code
}

template<typename STREAM>
void stream_cpp_init_common(STREAM* s, gmio_stream_t* stream)
{
    *stream = gmio_stream_null();
    stream->cookie = s;
    stream->func_at_end = gmio::internal::stream_cpp_at_end<STREAM>;
    stream->func_error = gmio::internal::stream_cpp_error<STREAM>;
}

} // namespace internal
} // namespace gmio

template<typename CHAR, typename TRAITS>
gmio_stream_t gmio_istream_cpp(std::basic_istream<CHAR, TRAITS>* s)
{
    typedef std::basic_istream<CHAR, TRAITS> CppStream;
    gmio_stream_t stream;
    gmio::internal::stream_cpp_init_common(s, &stream);
    stream.func_size = gmio::internal::istream_cpp_size<CppStream>;
    stream.func_read = gmio::internal::istream_cpp_read<CppStream>;
    stream.func_get_pos = gmio::internal::istream_cpp_get_pos<CppStream>;
    stream.func_set_pos = gmio::internal::istream_cpp_set_pos<CppStream>;
    return stream;
}

template<typename CHAR, typename TRAITS>
gmio_stream_t gmio_ostream_cpp(std::basic_ostream<CHAR, TRAITS>* s)
{
    typedef std::basic_ostream<CHAR, TRAITS> CppStream;
    gmio_stream_t stream;
    gmio::internal::stream_cpp_init_common(s, &stream);
    stream.func_size = gmio::internal::ostream_cpp_size<CppStream>;
    stream.func_write = gmio::internal::ostream_cpp_write<CppStream>;
    stream.func_get_pos = gmio::internal::ostream_cpp_get_pos<CppStream>;
    stream.func_set_pos = gmio::internal::ostream_cpp_set_pos<CppStream>;
    return stream;
}

#endif // !DOXYGEN

#endif /* GMIO_SUPPORT_STREAM_CPP_H */
/*! @} */
