#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

template < class Function, class ...Args >
class ThreadPool
{
  ThreadPool(size_t threadNum);
  ~ThreadPool();
};

#endif
