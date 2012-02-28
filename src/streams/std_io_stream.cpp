#include "std_io_stream.h"

namespace foug {

StdIoStream::StdIoStream(std::iostream* iostr)
  : m_iostr(iostr)
{
}

Int64 StdIoStream::read(char* data, Int64 maxSize)
{
  if (!m_iostr->eof())
    return 0;

  m_iostr->read(data, maxSize);
  if (!m_iostr->good())
    return -1;
  return maxSize;
}

Int64 StdIoStream::write(const char* data, Int64 maxSize)
{
  m_iostr->write(data, maxSize);
  if (!m_iostr->good())
    return -1;
  return maxSize;
}

} // namespace foug
