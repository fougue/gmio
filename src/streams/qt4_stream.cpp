#include "qt4_stream.h"

#include <QtCore/QIODevice>
#include <QtCore/QtDebug>

namespace foug {

Qt4Stream::Qt4Stream(QIODevice *device)
  : m_device(device)
{
}

bool Qt4Stream::atEnd() const
{
  return m_device->atEnd();
}

bool Qt4Stream::isWritable() const
{
  return m_device != 0 ? m_device->isWritable() : false;
}

bool Qt4Stream::isReadable() const
{
  return m_device != 0 ? m_device->isReadable() : false;
}

bool Qt4Stream::isSequential() const
{
  return m_device != 0 ? m_device->isSequential() : false;
}

bool Qt4Stream::seek(Int64 pos)
{
  return m_device->seek(pos);
}

Int64 Qt4Stream::read(char *data, Int64 maxSize)
{
  return m_device->read(data, maxSize);
}

Int64 Qt4Stream::write(const char *data, Int64 maxSize)
{
  return m_device->write(data, maxSize);
}

} // namespace foug
