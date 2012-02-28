#ifndef FOUG_ABSTRACT_STREAM_H
#define FOUG_ABSTRACT_STREAM_H

#include "foug_global.h"

namespace foug {

class AbstractStream
{
public:
  virtual bool atEnd() const = 0;

  virtual bool isWritable() const = 0;
  virtual bool isReadable() const = 0;
  virtual bool isSequential() const = 0;

  virtual bool seek(foug::Int64 pos) = 0;
  virtual Int64 read(char* data, Int64 maxSize) = 0;
  virtual Int64 write(const char* data, Int64 maxSize) = 0;
};

} // namespace foug

#endif // FOUG_ABSTRACT_STREAM_H
