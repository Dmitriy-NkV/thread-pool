#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <vector>
#include <queue>
#include <functional>
#include <memory>

class ThreadPool
{
public:
  ThreadPool(size_t threadNum);
  ~ThreadPool();

  template < class Function, class... Args >
  auto submit(Function&& function, Args&&... args) -> std::future< std::invoke_result_t< Function, Args... > >;
  void shutdown();
  void wait();
private:
  size_t threadNum_;
  std::queue< std::function< void() > > tasks_;
  std::vector< std::thread > threads_;
  std::atomic< bool > stop_;
  std::mutex tasksMutex_;
  std::condition_variable tasksCV_;

  void run();
};

ThreadPool::ThreadPool(size_t threadNum):
  threadNum_(std::thread::hardware_concurrency() ? std::min(static_cast< size_t >(std::thread::hardware_concurrency()), threadNum) : threadNum),
  tasks_(),
  threads_(),
  stop_(false),
  tasksMutex_(),
  tasksCV_()
{
  threads_.reserve(threadNum_);
  for (size_t i = 0; i != threadNum_; ++i)
  {
    threads_.emplace_back(&ThreadPool::run, this);
  }
}

void ThreadPool::run()
{
  while (!stop_)
  {
    std::function< void() > task;
    {
      std::unique_lock< std::mutex > lock(tasksMutex_);
      tasksCV_.wait(lock, [this]() -> bool { return stop_ || !tasks_.empty(); });
      if (stop_ && tasks_.empty())
      {
        return;
      }
      else
      {
        task = std::move(tasks_.front());
        tasks_.pop();
      }
    }
    task();
  }
}

template < class Function, class... Args >
auto ThreadPool::submit(Function&& function, Args&&... args) -> std::future< std::invoke_result_t< Function, Args... > >
{
  auto task = std::make_shared< std::packaged_task< std::invoke_result_t< Function, Args... >() > >(std::bind(std::forward< Function >(function), std::forward< Args >(args)...));
  auto res = task->get_future();
  {
    std::unique_lock< std::mutex > lock(tasksMutex_);
    tasks_.emplace([task]() { (*task)(); });
  }
  return res;
}

#endif
