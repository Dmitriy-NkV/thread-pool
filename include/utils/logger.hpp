#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <format>
#include <chrono>
#include <mutex>

namespace logger
{
  enum class LogLevel
  {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
  };

  class Logger
  {
  public:
    Logger() = default;
    ~Logger() = default;

    static Logger& get_instance();
    void log(LogLevel level, const std::string& message);

  private:
    std::mutex log_mutex_;

    constexpr std::string_view level_to_string(LogLevel level);
  };
}

#define LOG(level, message) \
  logger::Logger::get_instance().log(level, message);

#endif