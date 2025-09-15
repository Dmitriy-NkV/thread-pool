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

namespace threadpool
{
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

  template < class Function, class... Args >
  auto threadpool::ThreadPool::submit(Function&& function, Args&&... args) -> std::future< std::invoke_result_t< Function, Args... > >
  {
    auto task = std::make_shared< std::packaged_task< std::invoke_result_t< Function, Args... >() > >(std::bind(std::forward< Function >(function), std::forward< Args >(args)...));
    auto res = task->get_future();
    {
      std::unique_lock< std::mutex > lock(tasksMutex_);
      if (stop_)
      {
        throw std::runtime_error("Error: ThreadPool is stoped");
      }
      tasks_.emplace([task]() { (*task)(); });
    }
    tasksCV_.notify_one();
    return res;
  }
}

#endif
