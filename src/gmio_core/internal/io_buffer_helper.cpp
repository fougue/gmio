#include "io_buffer_helper.h"

namespace gmio {

IoBufferHelper::IoBufferHelper()
    : m_defaultIoBuff(std::move(IoBufferHelper::defaultIoBuffer())),
      m_buffer(makeSpan(m_defaultIoBuff))
{
}

IoBufferHelper::IoBufferHelper(Span<uint8_t> buffer)
{
    if (buffer.empty()) {
        m_defaultIoBuff = std::move(IoBufferHelper::defaultIoBuffer());
        m_buffer = makeSpan(m_defaultIoBuff);
    }
    else {
        m_buffer = buffer;
    }
}

std::vector<uint8_t> IoBufferHelper::defaultIoBuffer(size_t size)
{
    std::vector<uint8_t> buff;
    buff.resize(size);
    return buff;
}

} // namespace gmio
