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

#include <cstring>
#include <type_traits>
#include <utility>
#include <vector>

namespace gmio {

// Vector optimized for the case when the array is small
template<typename T, size_t STACK_SIZE>
class SmallVector {
public:
    ~SmallVector() {
        // Delete T objects from memstack
        for (size_t pos = 0; pos < m_size; ++pos)
            reinterpret_cast<T*>(m_memstack + pos)->~T();
    }

    void push_back(const T& value) {
        if (m_size < STACK_SIZE) {
            new(m_memstack + m_size) T(value);
        } else {
            this->switchToHeap();
            m_memheap.push_back(value);
        }
        ++m_size;
    }

    void push_back(T&& value) {
        if (m_size < STACK_SIZE) {
            new(m_memstack + m_size) T(std::forward<T>(value));
        } else {
            this->switchToHeap();
            m_memheap.push_back(std::forward<T>(value));
        }
        ++m_size;
    }

    template<typename... ARGS>
    void emplace_back(ARGS&&... args) {
        if (m_size < STACK_SIZE) {
            new(m_memstack + m_size) T(std::forward<ARGS>(args)...);
        } else {
            this->switchToHeap();
            m_memheap.emplace_back(std::forward<ARGS>(args)...);
        }
        ++m_size;
    }

    constexpr size_t size() const {
        return m_size;
    }

    constexpr const T* data() const {
        return m_size < STACK_SIZE ?
                    reinterpret_cast<const T*>(m_memstack) :
                    m_memheap.data();
    }

    T* data() {
        return m_size < STACK_SIZE ?
                    reinterpret_cast<T*>(m_memstack) :
                    m_memheap.data();
    }

private:
    void switchToHeap() {
        if (m_size == STACK_SIZE) {
            m_memheap.reserve(2 * STACK_SIZE);
            m_memheap.resize(STACK_SIZE);
            std::memcpy(m_memheap.data(), m_memstack, sizeof(T) * STACK_SIZE);
        }
    }

    // Type providing aligned uninitialized storage for T objects
    using T_AsBlock = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
    T_AsBlock m_memstack[STACK_SIZE];
    std::vector<T> m_memheap;
    size_t m_size = 0;
};

} // namespace gmio
