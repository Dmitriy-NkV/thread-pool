#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <vector>
#include <queue>

template < class Function, class... Args >
class ThreadPool
{
public:
  ThreadPool(size_t threadNum);
  ~ThreadPool();

  auto submit(Function&& function, Args&&... args) -> std::future< std::invoke_result_t< Function, Args... > >;
  void shutdown();
  void wait();
private:
  size_t threadNum_;
  std::queue< std::function< void() > > tasks_;
  std::vector< std::thread > threads_;
  std::atomic< bool > isWork_;
  std::mutex tasksMutex_;
  std::condition_variable tasksCV_;

  void run();
};

template < class Function, class... Args >
ThreadPool< Function, Args... >::ThreadPool(size_t threadNum):
  threadNum_(std::thread::hardware_concurrency() ? std::min(std::thread::hardware_concurrency(), threadNum) : threadNum),
  tasks_(),
  threads_(),
  isWork_(true),
  tasksMutex_(),
  tasksCV_()
{
  threads.reserve(threadNum_);
  for (size_t i = 0; i != threadNum_; ++i)
  {
    threads_.emplace_back(ThreadPool::run);
  }
}

template < class Function, class... Args >
void ThreadPool< Function, Args... >::run()
{
  while (isWork_)
  {
    std::function< void() > task;
    {
      std::unique_lock< std::mutex > lock(tasksMutex_);
      tasksCV_.wait(lock, [this]() -> bool { return !isWork_ || !tasks_.empty(); });
      if (stop && tasks.empty())
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

#endif
