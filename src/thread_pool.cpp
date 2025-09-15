#include "thread_pool.hpp"

threadpool::ThreadPool::ThreadPool(size_t threadNum):
  threadNum_(std::thread::hardware_concurrency() ? std::min(static_cast< size_t >(std::thread::hardware_concurrency()), threadNum) : threadNum),
  tasks_(),
  threads_(),
  stop_(false),
  tasksMutex_(),
  waitCV_(),
  threadsInWork_(0),
  tasksCV_()
{
  threads_.reserve(threadNum_);
  for (size_t i = 0; i != threadNum_; ++i)
  {
    threads_.emplace_back(&ThreadPool::run, this);
  }
}

void threadpool::ThreadPool::run()
{
  while (!stop_)
  {
    std::function< void() > task;
    {
      std::unique_lock< std::mutex > lock(tasksMutex_);
      if (tasks_.empty() && !threadsInWork_)
      {
        waitCV_.notify_one();
      }
      tasksCV_.wait(lock, [this]() -> bool { return stop_ || !tasks_.empty(); });
      if (stop_ && tasks_.empty())
      {
        return;
      }
      else
      {
        task = std::move(tasks_.front());
        ++threadsInWork_;
        tasks_.pop();
      }
    }
    task();
    --threadsInWork_;
  }
}

threadpool::ThreadPool::~ThreadPool()
{
  shutdown();
}

void threadpool::ThreadPool::shutdown()
{
  {
    std::unique_lock< std::mutex > lock(tasksMutex_);
    stop_ = true;
  }
  tasksCV_.notify_all();
  for (auto i = threads_.begin(); i != threads_.end(); ++i)
  {
    if (i->joinable())
    {
      i->join();
    }
  }
}

void threadpool::ThreadPool::wait()
{
  tasksCV_.notify_all();
  std::unique_lock< std::mutex > lock(tasksMutex_);
  waitCV_.wait(lock, [this]() -> bool { return tasks_.empty() && !threadsInWork_; });
}
