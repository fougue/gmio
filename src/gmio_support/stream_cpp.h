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

#ifndef GMIO_SUPPORT_STREAM_CPP_H
#define GMIO_SUPPORT_STREAM_CPP_H

#include "support_global.h"
#include "../gmio_core/stream.h"

#include <cstring>
#include <iostream>

template<typename STREAM>
gmio_stream_t gmio_stream_cpp(STREAM* s);

//
// Implementation
//

template<typename STREAM>
gmio_bool_t gmio_stream_cpp_at_end(void* cookie)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    return s->eof();
}

template<typename STREAM>
int gmio_stream_cpp_error(void* cookie)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    return s->rdstate();
}

template<typename STREAM>
size_t gmio_stream_cpp_read(
        void* cookie, void* ptr, size_t item_size, size_t item_count)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    s->read(static_cast<char*>(ptr), item_size * item_count);
    return s->gcount() / item_size;
}

template<typename STREAM>
size_t gmio_stream_cpp_write(
        void* cookie, const void* ptr, size_t item_size, size_t item_count)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    s->write(static_cast<const char*>(ptr), item_size * item_count);
    // TODO: return the number of bytes actually written
    return item_size * item_count;
}

template<typename STREAM>
size_t gmio_stream_cpp_size(void* cookie)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    std::streampos pos = s->tellg();
    s->seekg(0, std::ios_base::beg);
    std::streampos begin_pos = s->tellg();
    s->seekg(0, std::ios_base::end);
    std::streampos end_pos = s->tellg();
    s->seekg(pos, std::ios_base::beg);
    return end_pos - begin_pos;
}

template<typename STREAM>
int gmio_stream_cpp_get_pos(void* cookie, gmio_stream_pos_t* pos)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    std::streampos spos = s->tellg();
    std::memcpy(&pos->cookie[0], &spos, sizeof(std::streampos));
    return 0;
}

template<typename STREAM>
static int gmio_stream_cpp_set_pos(void* cookie, const gmio_stream_pos_t* pos)
{
    STREAM* s = static_cast<STREAM*>(cookie);
    std::streampos spos;
    std::memcpy(&spos, &pos->cookie[0], sizeof(std::streampos));
    s->seekg(spos);
    s->seekp(spos);
    return 0; // TODO: return error code
}

template<typename STREAM>
gmio_stream_t gmio_stream_cpp(STREAM* s)
{
    gmio_stream_t stream = gmio_stream_null();
    stream.cookie = s;
    stream.func_at_end = gmio_stream_cpp_at_end<STREAM>;
    stream.func_error = gmio_stream_cpp_error<STREAM>;
    stream.func_read = gmio_stream_cpp_read<STREAM>;
    stream.func_write = gmio_stream_cpp_write<STREAM>;
    stream.func_size = gmio_stream_cpp_size<STREAM>;
    stream.func_get_pos = gmio_stream_cpp_get_pos<STREAM>;
    stream.func_set_pos = gmio_stream_cpp_set_pos<STREAM>;
    return stream;
}

#endif /* GMIO_SUPPORT_STREAM_CPP_H */
/*! @} */
