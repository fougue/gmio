#pragma once

#include "../span.h"
#include <vector>

namespace gmio {

class IoBufferHelper {
public:
    IoBufferHelper();
    IoBufferHelper(Span<uint8_t> buffer);

    const Span<uint8_t>& get() const { return m_buffer; }

    static std::vector<uint8_t> defaultIoBuffer(size_t size = 512*1024 /*512KB*/);

private:
    std::vector<uint8_t> m_defaultIoBuff;
    Span<uint8_t> m_buffer;
};

} // namespace gmio
