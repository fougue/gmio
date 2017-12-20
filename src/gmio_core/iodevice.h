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

/*! \file iodevice.h
 *  Declaration of gmio::IoDevice
 *
 *  \addtogroup gmio_core
 *  @{
 */

#pragma once

#include "global.h"

#include <cstdio>
#include <functional>
#include <istream>
#include <ostream>

namespace gmio {

enum class ReadState {
    Good,
    Eof,
    Error
};

typedef std::function<uint64_t (uint8_t*, uint64_t, ReadState*)> FuncReadData;
typedef std::function<uint64_t (const uint8_t*, uint64_t)> FuncWriteData;

// <cstdio> FILE*
GMIO_API FuncReadData  FILE_funcReadData(FILE* file);
GMIO_API FuncWriteData FILE_funcWriteData(FILE* file);
GMIO_API uint64_t FILE_size(FILE* file);

// C++ iostream
GMIO_API FuncReadData  istream_funcReadData(std::istream* stream);
GMIO_API FuncWriteData ostream_funcWriteData(std::ostream* stream);

// Helpers
template<typename T>
bool readExactly(const FuncReadData& func_read, T* ptr, size_t count, ReadState* state = nullptr);

template<typename CONTAINER>
bool readExactly(const FuncReadData& func_read, CONTAINER* ctner, ReadState* state = nullptr);

#if 0
template<typename T, size_t N>
bool readExactly(const FuncReadData& func_read, T (&array)[N], ReadState* state = nullptr);
#endif

template<typename T>
bool writeExactly(const FuncWriteData& func_write, const T* ptr, size_t count);

template<typename CONTAINER>
bool writeExactly(const FuncWriteData& func_write, const CONTAINER& ctner);

template<typename T, size_t N>
bool writeExactly(const FuncWriteData& func_write, const T (&array)[N]);

//
// Implementation
//

//! Wraps 'func_read', ensuring that the read succeeded and exactly
//! 'sizeof(T) * count' bytes were read
template<typename T>
bool readExactly(const FuncReadData& func_read, T* ptr, size_t count, ReadState* state)
{
    const uint64_t size = sizeof(T) * count;
    return func_read(reinterpret_cast<uint8_t*>(ptr), size, state) == size;
}

//! Wraps 'func_read', ensuring that the read succeeded and exactly
//! 'sizeof(CONTAINER::value_type) * ctner->size()' bytes were read
template<typename CONTAINER>
bool readExactly(const FuncReadData& func_read, CONTAINER* ctner, ReadState* state)
{
    return readExactly(func_read, ctner->data(), ctner->size(), state);
}

#if 0
template<typename T, size_t N>
bool readExactly(const FuncReadData& func_read, T (&array)[N], ReadState* state)
{
    return readExactly(func_read, &array[0], N, state);
}
#endif

template<typename T>
bool writeExactly(const FuncWriteData& func_write, const T* ptr, size_t count)
{
    const uint64_t size = sizeof(T) * count;
    return func_write(reinterpret_cast<const uint8_t*>(ptr), size) == size;
}

template<typename CONTAINER>
bool writeExactly(const FuncWriteData& func_write, const CONTAINER& ctner)
{
    return writeExactly(func_write, ctner.data(), ctner.size());
}

template<typename T, size_t N>
bool writeExactly(const FuncWriteData& func_write, const T (&array)[N])
{
    return writeExactly(func_write, &array[0], N);
}

} // namespace gmio

/*! @} */
