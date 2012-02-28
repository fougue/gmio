#ifndef FOUG_QT4_STREAM_H
#define FOUG_QT4_STREAM_H

#include "abstract_stream.h"
class QIODevice;

namespace foug {

class Qt4Stream : public AbstractStream
{
public:
  Qt4Stream(QIODevice* device);

  bool atEnd() const;

  bool isWritable() const;
  bool isReadable() const;
  bool isSequential() const;

  bool seek(foug::Int64 pos);
  Int64 read(char* data, Int64 maxSize);
  Int64 write(const char* data, Int64 maxSize);

private:
  QIODevice* m_device;
};

} // namespace foug

#endif // FOUG_QT4_STREAM_H
