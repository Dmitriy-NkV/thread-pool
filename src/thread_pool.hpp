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
};

#endif
