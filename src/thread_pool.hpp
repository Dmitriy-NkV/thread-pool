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
  std::condition_variable waitCV_;
  std::atomic< size_t > threadsInWork_;
  std::condition_variable tasksCV_;

  void run();
};

#endif
