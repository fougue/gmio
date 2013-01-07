#ifndef FOUG_CPP_IO_BASE_H
#define FOUG_CPP_IO_BASE_H

namespace foug {

class AbstractStream;
class AbstractTaskProgress;

namespace internal { class IoBasePrivate; }

class IoBase
{
public:
  IoBase(AbstractStream* stream = 0);
  virtual ~IoBase();

  AbstractStream* stream() const;
  void setStream(AbstractStream* stream);

  AbstractTaskProgress* taskProgress() const;
  void setTaskProgress(AbstractTaskProgress* progress);

  bool autoDeleteStream() const;
  void setAutoDeleteStream(bool on);

  bool autoDeleteTaskProgress() const;
  void setAutoDeleteTaskProgress(bool on) const;

private:
  internal::IoBasePrivate* const d;
};

} // namespace foug

#endif // FOUG_CPP_IO_BASE_H
