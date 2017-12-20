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

#include "buffer_read_helper.h"

namespace gmio {

BufferReadHelper::BufferReadHelper(
        FuncReadData base_func_read, Span<uint8_t> initial_data)
    : m_base_func_read(std::move(base_func_read)),
      m_initial_data(initial_data)
{}

uint64_t BufferReadHelper::read(uint8_t *dst, uint64_t size, ReadState *state)
{
    uint64_t readlen = 0;
    if (m_readpos >= m_initial_data.size()) {
        readlen = m_base_func_read(dst, size, state);
    }
    else if (size <= (m_initial_data.size() - m_readpos)) {
        std::memcpy(dst, m_initial_data.data() + m_readpos, size);
        readlen = size;
        if (state != nullptr)
            *state = ReadState::Good;
    }
    else {
        const size_t remaining_len = m_initial_data.size() - m_readpos;
        std::memcpy(dst, m_initial_data.data() + m_readpos, remaining_len);
        readlen += remaining_len;
        readlen += m_base_func_read(dst + remaining_len, size - remaining_len, state);
    }
    m_readpos += readlen;
    return readlen;
}

FuncReadData BufferReadHelper_funcReadData(BufferReadHelper *helper)
{
    return [=](uint8_t* dst, uint64_t size, ReadState* state) {
        return helper->read(dst, size, state);
    };
}

} // namespace gmio
