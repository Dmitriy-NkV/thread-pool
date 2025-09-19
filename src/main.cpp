#include "thread_pool.hpp"

int get_count(int count)
{
  int ans = 0;
  while (ans != count)
  {
    ++ans;
  }
  return ans;
}

int main()
{
  try
  {
    threadpool::ThreadPool pool(6);
    std::vector< std::future< int > > futures;
    for (size_t i = 0; i != 6; ++i)
    {
      futures.push_back(pool.submit(get_count, 1000000000));
    }

    for (size_t i = 0; i != 6; ++i)
    {
      LOG(logger::LogLevel::INFO, std::format("Result {}: {}", i + 1, futures[i].get()));
    }
  }
  catch (const std::exception& e)
  {
    LOG(logger::LogLevel::CRITICAL, std::format("Error: {}", e.what()));
    return 1;
  }

  return 0;
}
