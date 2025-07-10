#ifndef THREAD_TASK_HPP
#define THREAD_TASK_HPP

#include <thread>
#include <mutex>
#include <condition_variable>

namespace detail
{
  template < class Function, class ...Args >
  class ThreadTask
  {
    ThreadTask(Function function, ...Args args);
    ~ThreadTask();
  };
}

#endif
