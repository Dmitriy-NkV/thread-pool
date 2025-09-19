#include "thread_pool.hpp"

threadpool::ThreadPool::ThreadPool(size_t threadNum):
  thread_num_(std::max(threadNum, static_cast< size_t >(1))),
  tasks_(),
  threads_(),
  stop_(false),
  tasks_mutex_(),
  wait_cv_(),
  threads_in_work_(0),
  tasks_cv_()
{
  threads_.reserve(thread_num_);
  for (size_t i = 0; i != thread_num_; ++i)
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
      std::unique_lock< std::mutex > lock(tasks_mutex_);
      if (tasks_.empty() && !threads_in_work_)
      {
        wait_cv_.notify_one();
      }
      tasks_cv_.wait(lock, [this]() -> bool { return stop_ || !tasks_.empty(); });
      if (stop_ && tasks_.empty())
      {
        return;
      }
      else
      {
        task = std::move(tasks_.front());
        ++threads_in_work_;
        tasks_.pop();
      }
    }
    task();
    --threads_in_work_;
  }
}

threadpool::ThreadPool::~ThreadPool()
{
  if (!stop_)
  {
    shutdown();
  }
}

void threadpool::ThreadPool::shutdown()
{
  {
    std::unique_lock< std::mutex > lock(tasks_mutex_);
    stop_ = true;
  }
  tasks_cv_.notify_all();
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
  tasks_cv_.notify_all();
  std::unique_lock< std::mutex > lock(tasks_mutex_);
  wait_cv_.wait(lock, [this]() -> bool { return tasks_.empty() && !threads_in_work_; });
}
