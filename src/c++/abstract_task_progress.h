#ifndef FOUG_CPP_ABSTRACT_TASK_PROGRESS_H
#define FOUG_CPP_ABSTRACT_TASK_PROGRESS_H

namespace foug {

namespace internal { class AbstractTaskProgressPrivate; }

class AbstractTaskProgress
{
public:
  AbstractTaskProgress();
  virtual ~AbstractTaskProgress();

  double rangeMin() const;
  double rangeMax() const;
  void setRange(double min, double max);

  int stepId() const;
  void setStepId(int id);

  double progress() const;
  double value() const;
  void setValue(double v);

  double progressUpdateThreshold() const;
  void setProgressUpdateThreshold(double v);

  virtual void reset();

  void asyncTaskStop();
  bool isTaskStopRequested() const;
  virtual void taskStoppedEvent();

  virtual void progressUpdateEvent() = 0;

private:
  internal::AbstractTaskProgressPrivate* const d;
};

} // namespace foug

#endif // FOUG_CPP_ABSTRACT_TASK_PROGRESS_H
