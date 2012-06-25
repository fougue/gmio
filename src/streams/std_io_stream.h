#ifndef FOUG_STD_IO_STREAM_H
#define FOUG_STD_IO_STREAM_H

#include "abstract_stream.h"

namespace foug {

template<typename STD_STREAM>
class StdInputStream : public AbstractStream
{
public:
  StdInputStream(STD_STREAM* istr)
    : m_istr(istr)
  {
  }

  bool atEnd() const
  {
    return m_istr->eof();
  }

  bool isWritable() const
  {
    return false;
  }

  bool isReadable() const
  {
    return true;
  }

  bool isSequential() const
  {
    return false;
  }

  bool seek(foug::Int64 pos)
  {
    m_istr->seekg(pos);
    return m_istr->good();
  }

  Int64 read(char* data, Int64 maxSize)
  {
    m_istr->read(data, maxSize);
    if (!m_istr->eof() && m_istr->fail()) {
      m_istr->clear();
      return -1;
    }
    const Int64 bytesCount = m_istr->gcount();
    if (m_istr->fail()) // TODO preserve eof() flag
      m_istr->clear();
    return bytesCount;
  }

  Int64 write(const char* /*data*/, Int64 /*maxSize*/)
  {
    return -1;
  }

private:
  STD_STREAM* m_istr;
};

template<typename STD_STREAM>
class StdOutputStream : public AbstractStream
{
public:
  StdOutputStream(STD_STREAM* ostr)
    : m_ostr(ostr)
  {
  }

  bool atEnd() const
  {
    return m_ostr->eof();
  }

  bool isWritable() const
  {
    return true;
  }

  bool isReadable() const
  {
    return false;
  }

  bool isSequential() const
  {
    return false;
  }

  bool seek(foug::Int64 pos)
  {
    m_ostr->seekp(pos);
    return m_ostr->good();
  }

  Int64 read(char* /*data*/, Int64 /*maxSize*/)
  {
    return -1;
  }

  Int64 write(const char* data, Int64 maxSize)
  {
    m_ostr->write(data, maxSize);
    if (!m_ostr->good())
      return -1;
    return maxSize;
  }

private:
  STD_STREAM* m_ostr;
};

} // namespace foug

#endif // FOUG_STD_IO_STREAM_H
